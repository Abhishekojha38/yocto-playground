DESCRIPTION = "Playground development image"
LICENSE = "MIT"

require recipes-core/images/core-image-minimal.bb

IMAGE_FEATURES += "\
    ssh-server-dropbear \
    tools-debug"

# Additional packages for Ollama
IMAGE_INSTALL:append = " \
    curl \
    ca-certificates \
    vim \
    htop \
    ncurses \
    procps \
"

IMAGE_INSTALL:append:playground-arm64 = " \
    ollama \
"

# Network configuration
IMAGE_INSTALL:append = " \
    iproute2 \
    iptables \
"

# Optional: Add tools for debugging and monitoring
IMAGE_INSTALL:append = " \
    strace \
    lsof \
    wget \
"

EXTRA_IMAGE_FEATURES += "empty-root-password"

# Add your custom package group
IMAGE_INSTALL += "packagegroup-base packagegroup-playground-base packagegroup-playground-graphics"

IMAGE_LINGUAS = "en-us"

# Set root password
inherit extrausers

PASSWD = "\$6\$8TnXaelqXPzojzNm\$K1uWk18gM/1ZsRsgJTaXxo52bdmFh.w49g9zac19mhBSod2wc6Gk8iHr1xl4QRiceIE5xt751wTsyIqAbnidC."

EXTRA_USERS_PARAMS = "\
    usermod -p '${PASSWD}' root; \
    "

# Extra space for models (approximately 5GB)
# Adjust this based on which models you plan to use
IMAGE_ROOTFS_EXTRA_SPACE = "5242880"
