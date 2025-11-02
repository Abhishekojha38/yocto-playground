DESCRIPTION = "Playground development image"
LICENSE = "MIT"

require recipes-core/images/core-image-minimal.bb

IMAGE_FEATURES += "ssh-server-dropbear tools-debug"

# Add your custom package group
IMAGE_INSTALL += "packagegroup-base"

IMAGE_LINGUAS = "en-us"

