DESCRIPTION = "Playground development image"
LICENSE = "MIT"

require recipes-core/images/core-image-minimal.bb

IMAGE_FEATURES += "\
    ssh-server-dropbear \
    tools-debug"

EXTRA_IMAGE_FEATURES += "empty-root-password"

# Add your custom package group
IMAGE_INSTALL += "packagegroup-base"

IMAGE_LINGUAS = "en-us"

# Set root password
inherit extrausers

PASSWD = "\$6\$8TnXaelqXPzojzNm\$K1uWk18gM/1ZsRsgJTaXxo52bdmFh.w49g9zac19mhBSod2wc6Gk8iHr1xl4QRiceIE5xt751wTsyIqAbnidC."

EXTRA_USERS_PARAMS = "\
    usermod -p '${PASSWD}' root; \
    "

