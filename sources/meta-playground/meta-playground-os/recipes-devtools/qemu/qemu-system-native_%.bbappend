FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://minimal_pcie_nic.c \
            file://meson.build \
            "

do_configure:prepend() {
    # Install the up-to-date C source directly into the QEMU source tree
    # NOTE: must be 'prepend' so the file is present before meson scans ${S}
    install -m 0644 ${WORKDIR}/sources-unpack/minimal_pcie_nic.c ${S}/hw/pci/minimal_pcie_nic.c

    # Install the up-to-date meson.build directly into the QEMU source tree
    install -m 0644 ${WORKDIR}/sources-unpack/meson.build ${S}/hw/pci/meson.build

    # Kconfig: add config symbol for the new device (idempotent)
    if ! grep -q 'MINIMAL_PCIE_NIC' ${S}/hw/pci/Kconfig; then
        cat >> ${S}/hw/pci/Kconfig <<'KCONFIG_EOF'

config MINIMAL_PCIE_NIC
    bool
    default y if TEST_DEVICES
    depends on PCI
KCONFIG_EOF
    fi
}

