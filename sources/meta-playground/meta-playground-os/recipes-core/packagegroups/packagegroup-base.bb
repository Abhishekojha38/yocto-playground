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
    pciutils \
    strace \
    usbutils \
    vim \
"

