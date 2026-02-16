SUMMARY = "Graphics package group for Playground"
LICENSE = "MIT"

inherit packagegroup

RDEPENDS:${PN} = " \
    weston \
    weston-init \
    weston-examples \
    wayland-utils \
"
