# meta-playground Recipes & Metadata Reference

This document describes the custom Yocto metadata under
`sources/meta-playground/`. It supplements the repository-root `AGENTS.md` (and
`~/.agents/AGENTS.md`); read those first for build commands, rules, and the
testing workflow. Edit files in this layer (not the upstream submodules) when
changing the platform.

`meta-playground` is split into two layers. Both are `LAYERSERIES_COMPAT` with
`walnascar`, depend on `core`, and have `BBFILE_PRIORITY` `10`.

## `meta-playground-os` (distro, image, packagegroups)

- **`conf/distro/playground-mini.conf`** — the `playground-mini` distro. Includes
  `poky.conf`; sets `DISTRO_VERSION = "1.0"`; adds `DISTRO_FEATURES` (wifi,
  bluetooth, systemd, wayland, pam, opengl, usrmerge); selects **systemd** as the
  init manager; enables `dbg-pkgs ssh-server-openssh package-management`; sets
  `IMAGE_FSTYPES = "tar.bz2 ext4"`. Edit here for distro-wide features/init/image
  format changes.
- **`recipes-core/images/playground-image.bb`** — the default image, `require`s
  `core-image-minimal.bb`. Adds SSH (dropbear), debug/profile tools, and packages
  via `IMAGE_INSTALL:append` (curl, ca-certificates, vim, htop, ncurses, procps,
  iproute2, iptables, iperf3, strace, lsof, wget). `ollama` is added only for
  `playground-arm64`. Pulls in `packagegroup-base`,
  `packagegroup-playground-base`, `packagegroup-playground-graphics`. Sets an
  empty/known **root password** via `inherit extrausers` + `EXTRA_USERS_PARAMS`
  (login `root`/`root`) and reserves ~5 GB rootfs extra space for models
  (`IMAGE_ROOTFS_EXTRA_SPACE`). Edit here to add/remove installed packages or
  change image features; keep BitBake override syntax (`:append`,
  `:append:playground-arm64`).
- **`recipes-core/packagegroups/packagegroup-playground-base.bb`** — base tools
  group (`inherit packagegroup`). `RDEPENDS:${PN}` = coreutils, devmem2, ethtool,
  gdb, git, htop, lsof, net-tools, pciutils, strace, usbutils, vim. Appends
  `minimal-pcie-nic` only for `playground-arm64`. Add general runtime tools here.
- **`recipes-core/packagegroups/packagegroup-playground-graphics.bb`** — graphics
  group; `RDEPENDS` = weston, weston-init, weston-examples, wayland-utils. Add
  Wayland/graphics packages here.

## `meta-playground-bsp` (machines, kernel, custom driver)

- **`conf/machine/playground-common.conf`** — shared machine settings; currently
  `KERNEL_IMAGETYPE = "Image"`. Put settings common to all machines here.
- **`conf/machine/playground-arm64.conf`** — includes `qemuarm64.conf` +
  `playground-common.conf`; sets `PREFERRED_PROVIDER_virtual/kernel =
  "linux-playground"`, `KBUILD_DEFCONFIG = "defconfig-arm64"`, and
  `MACHINEOVERRIDES`. This is the **default** `MACHINE`.
- **`conf/machine/playground-arm.conf`** — same pattern over `qemuarm.conf`,
  `defconfig-arm`.
- **`conf/machine/playground-x86.conf`** — same pattern over `qemux86-64.conf`,
  `defconfig-x86`. When editing machine configs, mirror the include +
  `PREFERRED_PROVIDER` + `KBUILD_DEFCONFIG` + `MACHINEOVERRIDES` structure.
- **`recipes-kernel/linux/linux-playground.inc`** — shared kernel logic:
  `inherit kernel`, native build DEPENDS (xz, bc, openssl, util-linux, gmp,
  libmpc, elfutils per ARCH), aarch64 toolchain tweaks. Put version-independent
  kernel settings here.
- **`recipes-kernel/linux/linux-playground_6.12.bb`** — mainline kernel `6.12.56`
  from kernel.org `linux-6.12.y` (`SRCREV = "v${PV}"`). `require`s the `.inc`,
  `PROVIDES += "virtual/kernel"`, sets `COMPATIBLE_MACHINE` per playground
  machine, and selects the per-machine defconfig via `SRC_URI:append:<machine>`.
  `do_configure:prepend` copies `${KBUILD_DEFCONFIG}` to `defconfig`;
  `do_compile:append` builds `scripts_gdb` when `CONFIG_GDB_SCRIPTS=y`. To bump
  the kernel, update `PV`/branch and adjust defconfigs.
- **`recipes-kernel/linux/linux-playground/defconfig-{arm,arm64,x86}`** —
  per-machine kernel configs consumed by the recipe above. Edit these to
  enable/disable kernel options for a machine.
- **`recipes-kernel/minimal-pcie-nic/minimal-pcie-nic_1.0.bb`** — out-of-tree
  kernel module (`inherit module`, `LICENSE = "CLOSED"`). Builds from
  `minimal-pcie-nic/minimal_pcie_nic_drv.c` + `Makefile`. Auto-loaded via
  `KERNEL_MODULE_AUTOLOAD`/`MODULE_AUTOLOAD = "minimal_pcie_nic_drv"`. Debug
  build flags `-DTRACE_PCIE_NIC -DDEBUG` are injected through
  `EXTRA_CFLAGS_DEBUG` → `EXTRA_OEMAKE`; remove them once the RX/TX path is
  verified. Installed to `${base_libdir}/modules/${KERNEL_VERSION}/extra/`. Only
  pulled into the image on `playground-arm64` (via the base packagegroup). Edit
  the driver `.c`/`Makefile` under the `minimal-pcie-nic/` subdirectory; the
  recipe uses `FILESEXTRAPATHS:prepend` to locate them.
