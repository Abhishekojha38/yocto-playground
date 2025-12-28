FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://0001-pcidev-add-minimal-pcie-dev.patch \
            file://0002-pcie-add-bar-mapping.patch \
            file://0003-pcie-enable-memory-access-and-bus-master.patch \
            file://0004-pci-add-msi-x-interrupt-support.patch \
            file://0005-pci-add-msi-and-msi-x-interrupt-generation-logic.patch \
            file://0006-pci-enable-and-test-msi-x-support.patch \
            "
