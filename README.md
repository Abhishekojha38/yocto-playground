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
    ├── meta-playground
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

Lets set the `MACHINE` and `DISTRO` config.

```bash
export DISTRO=playground-mini
export MACHINE=playground-x86
```

`OR`

```bash
cat << EOF > conf/local.conf
INHERIT += "buildhistory"
MACHINE = "playground-x86"
DISTRO = "playground-mini"
ERROR_QA:remove = "buildpaths"
EOF
```

```
bitbake playground-image
```

Build the SDK Installer

```
bitbake playground-image -c populate_sdk
```

Run the Installer

```
cd tmp/deploy/sdk/
 ./poky-glibc-x86_64-playground-image-core2-64-playground-x86-toolchain-1.0.sh
```
By default, Installer will be installed in /opt directory

```
playground-mini SDK installer version 1.0
=========================================
Enter target directory for SDK (default: /opt/poky/1.0):
You are about to install the SDK to "/opt/poky/1.0". Proceed [Y/n]?
Extracting SDK.....................................................................................................................................done
Setting it up...done
SDK has been successfully set up and is ready to be used.
Each time you wish to use the SDK in a new shell session, you need to source the environment setup script e.g.
 $ . /opt/poky/1.0/environment-setup-core2-64-poky-linux
```

---

## 🖥️ Using the Quick EMUlator (QEMU)

After building the image, you can boot and test it using QEMU, the Yocto
Project’s emulator.

Setting Up the Environment.

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

