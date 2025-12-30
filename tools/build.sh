#!/bin/bash
# Build script for AMP MCU Reference

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Default values
PLATFORM="generic"
BUILD_TYPE="Debug"
BUILD_DIR="build"
CLEAN=false

# Usage
usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Build the AMP MCU Reference implementation.

OPTIONS:
    -p, --platform PLATFORM    Target platform (generic, rp2350) [default: generic]
    -t, --type TYPE           Build type (Debug, Release) [default: Debug]
    -b, --build-dir DIR       Build directory [default: build]
    -c, --clean               Clean build directory before building
    -h, --help                Show this help message

EXAMPLES:
    $0                        # Build with defaults (generic, Debug)
    $0 -p rp2350 -t Release   # Build for RP2350 in Release mode
    $0 --clean                # Clean and rebuild

EOF
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -p|--platform)
            PLATFORM="$2"
            shift 2
            ;;
        -t|--type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -b|--build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo -e "${RED}Error: Unknown option $1${NC}"
            usage
            exit 1
            ;;
    esac
done

# Print configuration
echo -e "${BLUE}=== AMP MCU Reference Build ===${NC}"
echo "Platform:    $PLATFORM"
echo "Build Type:  $BUILD_TYPE"
echo "Build Dir:   $BUILD_DIR"
echo ""

# Clean if requested
if [ "$CLEAN" = true ]; then
    echo -e "${BLUE}Cleaning build directory...${NC}"
    rm -rf "$ROOT_DIR/$BUILD_DIR"
fi

# Configure
echo -e "${BLUE}Configuring with CMake...${NC}"
cd "$ROOT_DIR"
cmake -B "$BUILD_DIR" \
    -DAMP_PLATFORM="$PLATFORM" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

# Build
echo -e "${BLUE}Building...${NC}"
cmake --build "$BUILD_DIR"

# Success
echo ""
echo -e "${GREEN}=== Build Successful ===${NC}"
echo "Binaries are in: $BUILD_DIR/examples/"
echo ""

# List executables
if [ -d "$BUILD_DIR/examples" ]; then
    echo "Built examples:"
    ls -lh "$BUILD_DIR/examples/" | grep -E "(hello-amp|pingpong|shared-counter)" || true
fi
