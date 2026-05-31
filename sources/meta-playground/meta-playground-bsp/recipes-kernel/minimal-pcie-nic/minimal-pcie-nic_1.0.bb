FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
DESCRIPTION = "Minimal PCIe NIC driver with MSI-X"

LICENSE = "CLOSED"

SRC_URI = "file://minimal_pcie_nic_drv.c \
           file://Makefile"

# Build kernel module
inherit module

# Specify the kernel build path
KERNEL_MODULE_AUTOLOAD = "minimal_pcie_nic_drv"

S = "${WORKDIR}/sources-unpack"

# Debug flags:
#   -DTRACE_PCIE_NIC  enables the [TRACE] netdev_info() calls in the driver
#   -DDEBUG           enables kernel pr_debug() / dev_dbg() for this module
# Remove or comment out these flags once the RX/TX path is confirmed working.
EXTRA_CFLAGS_DEBUG = "-DTRACE_PCIE_NIC -DDEBUG"

EXTRA_OEMAKE += " \
    KDIR=${STAGING_KERNEL_DIR} \
    KVER=${KERNEL_VERSION} \
    EXTRA_CFLAGS=`${EXTRA_CFLAGS_DEBUG}` \
"

do_compile() {
    oe_runmake
}

do_install() {
    install -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra
    install -m 0644 minimal_pcie_nic_drv.ko \
        ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra/
}

MODULE_AUTOLOAD = "minimal_pcie_nic_drv"
INSANE_SKIP:${PN}-dbg += "buildpaths ldflags"
