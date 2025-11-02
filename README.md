# Yocto Playground

A sandbox environment for learning, experimenting, and performing R&D with the
Yocto Project.
This build is uisng **Walnascar** yocto release. Here is the detail about the
yocto release https://wiki.yoctoproject.org/wiki/Releases

---

## 🧩 Overview

This repository is designed to:
- Clone and manage Yocto layers using Git submodules.
- Build custom embedded Linux images for experimentation.
- Serve as a foundation for learning, prototyping, and feature development.

---

## 📁 Repository Structure

```
yocto-playground/
├── .gitmodules
├── .cqfd
├── build
├── README.md
└── sources
    ├── meta-openembedded
    └── poky
```

---

## 📋 Prerequisites

Before proceeding, ensure you have the following installed on your system:

- **Git** (for managing submodules)
- **Docker** (for isolated builds)
- **CQFD** — a container-based build tool for Yocto

To install CQFD:

```bash
curl -LO https://github.com/savoirfairelinux/cqfd/releases/download/v5.7.2/cqfd_5.7.2_all.deb
sudo dpkg -i ./cqfd_5.7.2_all.deb

```

---

## ⚙️ Setup Instructions

### 1️⃣ Clone this repository

```bash
git clone https://github.com/<your-username>/yocto-playground.git
cd yocto-playground
```

### 2️⃣ Initialize and update all submodules

```bash
git submodule update --init --recursive
```

---

## 🧰 Build Setup

### 1️⃣ Source the build environment

We are going to use `cqfd` to build the yocto in docker.

Now prepare and build the yocto `qemu` image.

```bash
export CQFD_SHELL=/bin/bash
```

```bash
cqfd init
cqfd shell
source sources/poky/oe-init-build-env
bitbake-layers add-layer ../sources/meta-playground/meta-playground-os
bitbake-layers add-layer ../sources/meta-playground/meta-playground-bsp
bitbake-layers add-layer ../sources/meta-openembedded/meta-oe
```

Build `playground-image` image.

```bash
export DISTRO=playground-mini
export MACHINE=playground-x86
bitbake playground-image
```

---

## 🖥️ Run QEMU

After building the image, you can boot and test it using QEMU, the Yocto
Project’s emulator.

Setup the environment.

```bash
source sources/poky/oe-init-build-env
```

Launch QEMU in nographic mode.

```bash
runqemu playground-x86 nographic slirp
```

---

## 💡 Tips
- Keep each layer on the same Yocto release branch (e.g., *kirkstone*, *mickledore*, *nanbield*).
- Use `conf/local.conf` for customizing build options.
---

## 🧑‍💻 Author
**Abhishek Ojha**  
Abhishekojha38@gmail.com

---

