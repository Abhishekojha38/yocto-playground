DESCRIPTION = "Package group for Playground tools and utilities"
LICENSE = "MIT"

inherit packagegroup

RDEPENDS:${PN} = " \
    vim \
    git \
    htop \
    strace \
    lsof \
    ethtool \
    gdb \
    usbutils \
    pciutils \
    coreutils \
"

