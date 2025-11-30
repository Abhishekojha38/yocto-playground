#!/bin/sh
set -e

# Default values
BUILD_SDK=0

# Parse arguments
while [ "$#" -gt 0 ]; do
    case "$1" in
        --sdk|-s)
            BUILD_SDK=1
            shift
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--sdk|-s]"
            exit 1
            ;;
    esac
done

# Read configuration
if [ -f "build.conf" ]; then
    . ./build.conf
else
    echo "Error: build.conf not found."
    exit 1
fi

if [ -z "$MACHINE" ] || [ -z "$DISTRO" ]; then
    echo "Error: MACHINE and DISTRO must be set in build.conf"
    exit 1
fi

echo "Building for MACHINE=$MACHINE DISTRO=$DISTRO"
if [ "$BUILD_SDK" -eq 1 ]; then
    echo "SDK build enabled"
fi

# Source the build environment
# We need to be in the directory of the script for it to work correctly with sh
# In dash (default /bin/sh on Debian/Ubuntu), . command does not accept arguments.
# It uses the current positional parameters. So we must set them.
cd sources/poky
set -- ../../build
. ./oe-init-build-env
# DO NOT cd back. We want to stay in the build directory.

# Add layers
# We are now in the build directory, so paths are relative to build/
# sources is at ../sources
bitbake-layers add-layer ../sources/meta-playground/meta-playground-os || true
bitbake-layers add-layer ../sources/meta-playground/meta-playground-bsp || true
bitbake-layers add-layer ../sources/meta-openembedded/meta-oe || true

# Run build
bitbake playground-image

if [ "$BUILD_SDK" -eq 1 ]; then
    echo "Building SDK..."
    bitbake playground-image -c populate_sdk
fi
