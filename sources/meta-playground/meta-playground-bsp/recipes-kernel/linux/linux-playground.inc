DESCRIPTION = "Custom Yocto kernel recipe for Playground machines (x86,ARM,etc.)"
SECTION = "kernel"
LICENSE = "GPL-2.0-only"

DEPENDS += "xz-native bc-native"
DEPENDS:append:aarch64 = " libgcc"
KERNEL_CC:append:aarch64 = " ${TOOLCHAIN_OPTIONS}"
KERNEL_LD:append:aarch64 = " ${TOOLCHAIN_OPTIONS}"

inherit kernel

# tools / dependencies will continue to be added in version specific recipes.
DEPENDS += '${@bb.utils.contains_any("ARCH", [ "x86", "arm64", "powerpc" ], "elfutils-native", "", d)}'
DEPENDS += "openssl-native util-linux-native"
DEPENDS += "gmp-native libmpc-native"

