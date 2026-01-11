DESCRIPTION = "Package group for Playground tools and utilities"
LICENSE = "MIT"

inherit packagegroup

RDEPENDS:${PN} = " \
    bpftool \
    coreutils \
    devmem2 \
    ethtool \
    gdb \
    git \
    htop \
    lsof \
    minimal-pcie-nic \
    net-tools \
    pciutils \
    strace \
    usbutils \
    vim \
"

