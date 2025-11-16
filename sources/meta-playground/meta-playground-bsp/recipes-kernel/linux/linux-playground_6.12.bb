FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
SUMMARY = "Custom Linux kernel for Playground platform"
DESCRIPTION = "Custom Yocto kernel recipe for Playground machines (x86, ARM,etc.)"
SECTION = "kernel"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

inherit kernel
require recipes-kernel/linux/linux-playground.inc

# Version info
PV = "6.12.56"
SRCREV = "v${PV}"

# Source repository (example using kernel.org mainline)
SRC_URI = "git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git;protocol=https;branch=linux-6.12.y"

# Use your own defconfig
SRC_URI:append:playground-arm64 = " file://defconfig-arm64"
SRC_URI:append:playground-x86 = " file://defconfig-x86"

S = "${WORKDIR}/git"

# Compatible machines — ensure your MACHINE includes this pattern
COMPATIBLE_MACHINE = "playground-*"

# Ensure your recipe provides virtual/kernel
PROVIDES += "virtual/kernel"

do_compile:append() {
   if (grep -q -i -e '^CONFIG_GDB_SCRIPTS=y$' .config && grep -q -e "^PHONY +=.*scripts_gdb" "${S}/Makefile"); then
       oe_runmake ${PARALLEL_MAKE} scripts_gdb
   fi
}
