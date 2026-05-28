// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#define DRV_NAME            "minimal_pcie_nic_drv"
#define VENDOR_ID           0x1af4
#define DEVICE_ID           0x10f1
#define MAX_MSI_VECTORS     4 // define in qemu pci device minimal_pci_nic
#define MAX_MSIX_VECTORS    4 // define in qemu pci device minimal_pci_nic

#define MSIX_ENABLE

/* Ring Configurations */
#define REG_RX_RING_BASE   0x10
#define REG_RX_RING_SIZE   0x18
#define REG_RX_TAIL        0x1C
#define REG_RX_HEAD        0x20

#define RX_RING_SIZE        16
#define RX_BUF_SIZE         2048
#define RX_DONE             1

/* QEMU NIC reads and writes exactly this layout using PCIe DMA */
struct rx_desc {
    u64 addr;   // where NIC must DMA the packet
    u16 len;    // length written by NIC
    u16 flags;  // DONE bit from NIC
};

struct minimal_dev {
    struct pci_dev *pdev;
    void __iomem *bar0;    // MMIO registers
    void __iomem *bar1;    // MSI-X table/PBA (optional mapping)
    int nvec_irq;
    struct net_device *netdev;

    struct rx_desc *rx_ring;   // Virtual address where Linux sees the descriptor ring
    dma_addr_t rx_ring_dma;     // Physical address QEMU NIC uses to access ring

    void *rx_bufs[RX_RING_SIZE];    // Actual packet buffers linux will use to access data
    dma_addr_t rx_bufs_dma[RX_RING_SIZE];   // Physical addresses of those buffers
};

static int minimal_open(struct net_device *ndev)
{
    netif_start_queue(ndev);
    return 0;
}

static int minimal_stop(struct net_device *ndev)
{
    netif_stop_queue(ndev);
    return 0;
}

static netdev_tx_t minimal_start_xmit(struct sk_buff *skb,
                                      struct net_device *ndev)
{
    /* Later you will DMA this into QEMU */
    dev_kfree_skb(skb);
    return NETDEV_TX_OK;
}

static const struct net_device_ops minimal_netdev_ops = {
    .ndo_open       = minimal_open,
    .ndo_stop       = minimal_stop,
    .ndo_start_xmit = minimal_start_xmit,
};

static irqreturn_t minimal_irq_handler(int irq, void *dev_id)
{
    struct minimal_dev *mdev = dev_id;
    u32 head = readl(mdev->bar0 + REG_RX_HEAD);
    u32 tail = readl(mdev->bar0 + REG_RX_TAIL);
    u32 i = tail;

    /* Walk only newly-completed descriptors from tail up to head. */
    while (i != head) {
        if (mdev->rx_ring[i].flags & RX_DONE) {
            pr_info("minimal_pcie_nic: RX[%u] len=%u\n", i, mdev->rx_ring[i].len);
            mdev->rx_ring[i].flags = 0;
        }
        i = (i + 1) % RX_RING_SIZE;
    }
    return IRQ_HANDLED;
}

static int minimal_probe(struct pci_dev *pdev,
                         const struct pci_device_id *id)
{
    struct minimal_dev *mdev;
    struct net_device *ndev;
    int ret, i;

    pr_info(DRV_NAME ": probe\n");

    ndev = alloc_etherdev(sizeof(*mdev));
    if (!ndev)
        return -ENOMEM;

    mdev = netdev_priv(ndev);
    mdev->pdev = pdev;
    mdev->netdev = ndev;
    pci_set_drvdata(pdev, ndev);  /* store ndev; mdev = netdev_priv(ndev) */

    ndev->netdev_ops = &minimal_netdev_ops;
    ndev->min_mtu = 68;
    ndev->max_mtu = 1500;

    eth_hw_addr_random(ndev);
    SET_NETDEV_DEV(ndev, &pdev->dev);

    /* Enable PCI device and bus-mastering before any hardware access */
    pr_info(DRV_NAME ": PCI enable device\n");
    ret = pci_enable_device(pdev);
    if (ret)
        goto err_free_netdev;

    pci_set_master(pdev);

    ret = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));
    if (ret) {
        dev_err(&pdev->dev, "Cannot set 64-bit DMA mask\n");
        goto err_disable;
    }

#ifdef MSIX_ENABLE
    /* Request MSI-X vectors explicitly */
    mdev->nvec_irq = pci_alloc_irq_vectors(pdev,
                                           1,
                                           MAX_MSIX_VECTORS,
                                           PCI_IRQ_MSIX);
#else
    /* Fallback to MSI with per-vector masking */
    mdev->nvec_irq = pci_alloc_irq_vectors(pdev,
                                1,
                                MAX_MSI_VECTORS,
                                PCI_IRQ_MSI);
#endif

    if (mdev->nvec_irq < 0) {
        ret = mdev->nvec_irq;
        goto err_disable;
    }

    for (i = 0; i < mdev->nvec_irq; i++) {
        int irq = pci_irq_vector(pdev, i);

        ret = devm_request_irq(&pdev->dev,
                               irq,
                               minimal_irq_handler,
                               0,
                               DRV_NAME,
                               mdev);
        if (ret) {
            dev_err(&pdev->dev, "IRQ %d request failed\n", i);
            goto err_irq;
        }
    }

    /* Map BAR0 (device MMIO) */
    ret = pci_request_region(pdev, 0, DRV_NAME);
    if (ret)
        goto err_irq;

    mdev->bar0 = pci_iomap(pdev, 0, 0);
    if (!mdev->bar0) {
        ret = -ENOMEM;
        goto err_region0;
    }

    /* Map BAR1 (MSI-X table/PBA) */
    ret = pci_request_region(pdev, 1, DRV_NAME);
    if (ret)
        goto err_unmap_bar0;

    mdev->bar1 = pci_iomap(pdev, 1, 0);
    if (!mdev->bar1) {
        ret = -ENOMEM;
        goto err_region1;
    }

    /* Allocate RX ring
     * 1. Allocates memory
     * 2. Makes it visible to the device
     * Returns the physical DMA address
     */
    mdev->rx_ring = dma_alloc_coherent(&pdev->dev,
            sizeof(struct rx_desc) * RX_RING_SIZE,
            &mdev->rx_ring_dma, GFP_KERNEL);

    pr_info("RX ring VA=%p  DMA=%pad  size=%zu\n",
        mdev->rx_ring,
        &mdev->rx_ring_dma,
        sizeof(struct rx_desc) * RX_RING_SIZE);

    /* Allocate 16 packet buffers of 2048 bytes each */
    for (i = 0; i < RX_RING_SIZE; i++) {
        mdev->rx_bufs[i] = dma_alloc_coherent(&pdev->dev,
                RX_BUF_SIZE,
                &mdev->rx_bufs_dma[i],
                GFP_KERNEL);

        mdev->rx_ring[i].addr = mdev->rx_bufs_dma[i];
        mdev->rx_ring[i].len = RX_BUF_SIZE;
        mdev->rx_ring[i].flags = 0;

        pr_info("RX[%02d] desc_va=%p  buf_va=%p  buf_dma=%pad\n",
            i,
            &mdev->rx_ring[i],
            mdev->rx_bufs[i],
            &mdev->rx_bufs_dma[i]);
    }

    /* Program device: use writeq for the 64-bit ring base address */
    writeq(mdev->rx_ring_dma,  mdev->bar0 + REG_RX_RING_BASE);
    writel(RX_RING_SIZE,       mdev->bar0 + REG_RX_RING_SIZE);
    writel(RX_RING_SIZE - 1,   mdev->bar0 + REG_RX_TAIL);

    pr_info(DRV_NAME ": BAR0=%p BAR1=%p IRQ vectors=%d\n",
            mdev->bar0, mdev->bar1, mdev->nvec_irq);

    /* Register netdev last, after all hardware resources are ready */
    ret = register_netdev(ndev);
    if (ret)
        goto err_dma;

    pr_info(DRV_NAME ": registered netdev %s\n", ndev->name);

    return 0;

err_dma:
    for (i = 0; i < RX_RING_SIZE; i++) {
        if (mdev->rx_bufs[i])
            dma_free_coherent(&pdev->dev, RX_BUF_SIZE,
                              mdev->rx_bufs[i], mdev->rx_bufs_dma[i]);
    }
    if (mdev->rx_ring)
        dma_free_coherent(&pdev->dev,
                          sizeof(struct rx_desc) * RX_RING_SIZE,
                          mdev->rx_ring, mdev->rx_ring_dma);
err_region1:
    if (mdev->bar1)
        pci_iounmap(pdev, mdev->bar1);
    pci_release_region(pdev, 1);
err_unmap_bar0:
    pci_iounmap(pdev, mdev->bar0);
err_region0:
    pci_release_region(pdev, 0);
err_irq:
    pci_free_irq_vectors(pdev);
err_disable:
    pci_disable_device(pdev);
err_free_netdev:
    free_netdev(ndev);
    return ret;
}

/* Remove */
static void minimal_remove(struct pci_dev *pdev)
{
    struct net_device *ndev = pci_get_drvdata(pdev);
    struct minimal_dev *mdev = netdev_priv(ndev);
    int i;

    /* Unregister first so no new traffic arrives while we tear down. */
    unregister_netdev(ndev);

    pr_info(DRV_NAME ": remove\n");
    for (i = 0; i < RX_RING_SIZE; i++) {
        if (mdev->rx_bufs[i])
            dma_free_coherent(&pdev->dev, RX_BUF_SIZE,
                              mdev->rx_bufs[i],
                              mdev->rx_bufs_dma[i]);
    }

    if (mdev->rx_ring)
        dma_free_coherent(&pdev->dev,
                          sizeof(struct rx_desc) * RX_RING_SIZE,
                          mdev->rx_ring,
                          mdev->rx_ring_dma);

    if (mdev->bar1)
        pci_iounmap(pdev, mdev->bar1);
    pci_release_region(pdev, 1);

    if (mdev->bar0)
        pci_iounmap(pdev, mdev->bar0);
    pci_release_region(pdev, 0);

    pci_free_irq_vectors(pdev);
    pci_disable_device(pdev);

    /* free_netdev last: mdev is embedded via netdev_priv, freed with ndev. */
    free_netdev(ndev);
}

/* PCI ID Table */
static const struct pci_device_id minimal_pci_ids[] = {
    { PCI_DEVICE(VENDOR_ID, DEVICE_ID) },
    { }
};
MODULE_DEVICE_TABLE(pci, minimal_pci_ids);

/* PCI Driver */
static struct pci_driver minimal_pci_driver = {
    .name     = DRV_NAME,
    .id_table = minimal_pci_ids,
    .probe    = minimal_probe,
    .remove   = minimal_remove,
};

module_pci_driver(minimal_pci_driver);

MODULE_AUTHOR("Abhishek Ojha");
MODULE_DESCRIPTION("Minimal PCIe NIC driver");
MODULE_LICENSE("GPL");
