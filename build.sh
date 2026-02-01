#!/bin/sh
set -e

# Get the absolute path to the script directory
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

# Default values
CUSTOM_COMMAND=""

# Parse arguments
while [ "$#" -gt 0 ]; do
    case "$1" in
        --)
            shift
            CUSTOM_COMMAND="$*"
            break
            ;;
        -*)
            echo "Unknown option: $1"
            echo "Usage: $0 [[--] command...]"
            exit 1
            ;;
        *)
            CUSTOM_COMMAND="$*"
            break
            ;;
    esac
done

# Read configuration
if [ -f "$SCRIPT_DIR/build.conf" ]; then
    . "$SCRIPT_DIR/build.conf"
else
    echo "Error: build.conf not found."
    exit 1
fi

if [ -z "$MACHINE" ] || [ -z "$DISTRO" ]; then
    echo "Error: MACHINE and DISTRO must be set in build.conf"
    exit 1
fi

echo "Building for MACHINE=$MACHINE DISTRO=$DISTRO"


if [ -n "$CUSTOM_COMMAND" ]; then
    echo "Custom command: $CUSTOM_COMMAND"
fi

# Source the build environment
# We need to be in the directory of the script for it to work correctly with sh
# In dash (default /bin/sh on Debian/Ubuntu), . command does not accept arguments.
# It uses the current positional parameters. So we must set them.
cd "$SCRIPT_DIR/sources/poky"
set -- ../../build-"${MACHINE}"
. ./oe-init-build-env

echo "Build directory: build-"${MACHINE}" "
# DO NOT cd back. We want to stay in the build directory.

# Append machine and distro to local.conf
cat <<EOF > conf/local.conf
DISTRO = "${DISTRO}"
MACHINE = "${MACHINE}"
EOF

# Disable QA errors
cat <<EOF >> conf/local.conf
ERROR_QA:remove = "buildpaths"
WARN_QA:append = " buildpaths"
ERROR_QA:remove = "patch-status"
WARN_QA:append = " patch-status"
EOF

# Add layers from layers.conf
LAYERS_CONF="$SCRIPT_DIR/layers.conf"
if [ -f "$LAYERS_CONF" ]; then
    echo "Adding layers from $LAYERS_CONF..."
    # Read ignoring comments and empty lines
    grep -vE '^\s*#|^\s*$' "$LAYERS_CONF" | while read -r layer; do
        echo "Adding layer: $layer"
        bitbake-layers add-layer "$layer" || true
    done
else
    echo "Warning: layers.conf not found at $LAYERS_CONF"
fi

# Execute custom command if provided
if [ -n "$CUSTOM_COMMAND" ]; then
    echo "Executing: $CUSTOM_COMMAND"
    eval "$CUSTOM_COMMAND"
    exit $?
fi

# Run build
bitbake playground-image
