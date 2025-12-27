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

EXTRA_OEMAKE += " \
    KDIR=${STAGING_KERNEL_DIR} \
    KVER=${KERNEL_VERSION} \
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
