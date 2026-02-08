DESCRIPTION = "Package group for Playground tools and utilities"
LICENSE = "MIT"

inherit packagegroup

RDEPENDS:${PN} = " \
    coreutils \
    devmem2 \
    ethtool \
    gdb \
    git \
    htop \
    lsof \
    net-tools \
    pciutils \
    strace \
    usbutils \
    vim \
"

RDEPENDS:${PN}:append:playground-arm64 = " minimal-pcie-nic"
