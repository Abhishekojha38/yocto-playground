# Yocto Layer Developer Agent Instructions

This document instructs AI coding agents working in the `yocto-playground`
repository. It is a sandbox for learning, experimenting, and performing R&D with
the Yocto Project, targeting the **Walnascar** Yocto release. Follow these
instructions to make correct, minimal, and buildable changes.

> **Also read `~/.agents/AGENTS.md`.** The rules and conventions defined in that
> file apply in addition to this document and must be followed together with the
> instructions here. If guidance ever conflicts, the repository-specific rules in
> this file take precedence for work in `yocto-playground`.

## Tech Stack & Ecosystem

- **Build system:** Yocto Project / OpenEmbedded (`bitbake`), Walnascar release.
- **Metadata:** BitBake recipes (`.bb`, `.bbappend`), classes (`.bbclass`), and
  configuration (`.conf`) files. Recipe/config language is BitBake + shell +
  Python.
- **Containerized builds:** [`cqfd`](https://github.com/savoirfairelinux/cqfd)
  runs the build inside a Docker container defined in `.cqfd/docker/Dockerfile`
  (Ubuntu 22.04 base). Configuration lives in `.cqfdrc`.
- **Layers** are managed as Git submodules under `sources/` (see `.gitmodules`):
  - `sources/poky` — core Poky metadata and `oe-init-build-env`.
  - `sources/meta-openembedded` — `meta-oe`, `meta-python`, etc.
  - `sources/meta-playground` — custom layers: `meta-playground-os` (distro,
    images, packagegroups) and `meta-playground-bsp` (machines, kernel, custom
    PCI device).
  - `sources/meta-ollama`, `sources/meta-llama-cpp`, `sources/meta-ollama-cpp` —
    AI/LLM recipe layers.
- **Distro:** `playground-mini` (systemd, usrmerge, wayland; see
  `sources/meta-playground/meta-playground-os/conf/distro/playground-mini.conf`).
- **Machines:** `playground-arm64` (default), `playground-arm`, `playground-x86`
  (QEMU-based, defined in `meta-playground-bsp/conf/machine/`).
- **Default image:** `playground-image` (extends `core-image-minimal`).
- **Emulation:** QEMU via `runqemu`.

## meta-playground Recipes & Metadata Reference

Detailed, per-recipe documentation for the custom layers under
`sources/meta-playground/` lives in **`sources/meta-playground/AGENTS.md`**.
Read that file before modifying any recipe, distro, machine, packagegroup, or
kernel/driver metadata in those layers.

## Environment & Executable Commands

Build configuration is driven by `build.conf` (`MACHINE`, `DISTRO`, `IMAGE`) and
the layer list in `layers.conf`. The `build.sh` script sources the build
environment, writes `conf/local.conf`, adds the layers from `layers.conf`, and
runs `bitbake`.

- **Initialize submodules (required first step):**
  ```bash
  git submodule update --init --recursive
  ```
- **Prepare the container (one-time):**
  ```bash
  cqfd init
  ```
- **Build the default image (uses `build.conf` + `build.sh`):**
  ```bash
  cqfd run
  ```
- **Run an arbitrary build command inside the container:**
  ```bash
  cqfd run ./build.sh -- bitbake playground-image
  ```
- **Open an interactive shell in the container:**
  ```bash
  export CQFD_SHELL=/bin/bash
  cqfd shell
  ```
- **Build the SDK / toolchain installer:**
  ```bash
  cqfd run ./build.sh -- bitbake playground-image -c populate_sdk
  ```
- **Source the environment manually (outside cqfd, for `runqemu`/inspection):**
  ```bash
  source sources/poky/oe-init-build-env
  ```
- **Boot the image in QEMU (nographic):**
  ```bash
  runqemu playground-arm64 nographic slirp
  # or: runqemu playground-x86 nographic slirp
  ```

Build output lands in `build-${MACHINE}/` (e.g. `build-playground-arm64/`).
Default QEMU credentials: user `root`, password `root`.

## Rules & Boundaries

- **Do not edit submodule contents** under `sources/poky` or
  `sources/meta-openembedded`. These are upstream mirrors; make customizations in
  `meta-playground` layers via `.bbappend` files or new recipes instead.
- **Keep all layers on the same Yocto release branch** (Walnascar). Do not bump a
  single submodule to a different release.
- **Prefer layered customization:** add machine/distro/image changes to the
  appropriate file under `sources/meta-playground/`, not to generated
  `conf/local.conf` (which `build.sh` overwrites on every run).
- **When adding a layer,** add it to `layers.conf` (path relative to the build
  directory, e.g. `../sources/meta-<name>`) and, if it is a new remote, register
  it as a submodule in `.gitmodules`.
- **Respect BitBake override syntax** (e.g. `IMAGE_INSTALL:append`,
  `:playground-arm64`) — do not convert to older `_append` syntax.
- **Do not commit build artifacts** (`build*/`, `tmp/`, `sstate-cache`,
  downloads) or secrets. Keep commits scoped to metadata and configuration.
- **Do not hardcode absolute host paths** in recipes or configuration.
- Builds are expensive and network-heavy; run them inside `cqfd` and avoid
  unnecessary full rebuilds.

## Testing & Verification Workflow

There is no unit-test suite; verification is build- and boot-based. After any
change:

1. **Sanity-check metadata parsing** (fast, catches syntax errors):
   ```bash
   cqfd run ./build.sh -- bitbake-layers show-layers
   ```
2. **Build the affected target** (or the full image for broad changes):
   ```bash
   cqfd run ./build.sh -- bitbake playground-image
   ```
   For a single recipe, substitute the recipe name (e.g.
   `bitbake <recipe>`), and use task flags such as `-c compile` or
   `-c do_rootfs` to narrow scope.
3. **Confirm the build completes** with no errors and that the image appears
   under `build-${MACHINE}/tmp/deploy/images/${MACHINE}/`.
4. **Boot-test in QEMU** for image/kernel/BSP changes:
   ```bash
   source sources/poky/oe-init-build-env build-playground-arm64
   runqemu playground-arm64 nographic slirp
   ```
   Log in as `root` / `root` and verify the changed functionality (installed
   packages, services, networking, etc.).

   For **`minimal-pcie-nic` or QEMU device changes**, launch QEMU with the
   custom device attached to a tap netdev instead of `slirp`:
   ```bash
   runqemu playground-arm64 nographic qemuparams="-netdev tap,id=net1,ifname=tap2,script=no,downscript=no -device minimal-pcie-nic,netdev=net1"
   ```
   Then confirm the driver bound and the interface came up (e.g. `dmesg | grep
   -i pcie_nic`, `lspci`, `ip link`).
5. For a broad or release change, repeat the build for the other supported
   machines (`playground-x86`, `playground-arm`) by updating `MACHINE` in
   `build.conf`.

Only consider a change complete once the relevant target builds cleanly and, for
runtime changes, boots and behaves as expected in QEMU.
