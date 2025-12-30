#!/bin/bash
# Flash script for RP2350 (Pico 2)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Default values
EXAMPLE=""
BUILD_DIR="build"
MOUNT_POINT=""

usage() {
    cat << EOF
Usage: $0 [OPTIONS] EXAMPLE

Flash an example to RP2350 (Pico 2) board.

ARGUMENTS:
    EXAMPLE                   Example to flash (hello-amp, pingpong, shared-counter)

OPTIONS:
    -b, --build-dir DIR       Build directory [default: build]
    -m, --mount MOUNT         Mount point of Pico board (auto-detected if not specified)
    -h, --help                Show this help message

EXAMPLES:
    $0 hello-amp              # Auto-detect mount point and flash
    $0 -m /media/RPI-RP2 pingpong

NOTES:
    - Hold BOOTSEL button while connecting Pico 2 to enter bootloader mode
    - The board will appear as a USB mass storage device (RPI-RP2)
    - After flashing, the board will reboot automatically

EOF
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -b|--build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        -m|--mount)
            MOUNT_POINT="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        -*)
            echo -e "${RED}Error: Unknown option $1${NC}"
            usage
            exit 1
            ;;
        *)
            EXAMPLE="$1"
            shift
            ;;
    esac
done

# Validate example name
if [ -z "$EXAMPLE" ]; then
    echo -e "${RED}Error: Example name required${NC}"
    usage
    exit 1
fi

# Find .uf2 file
UF2_FILE="$ROOT_DIR/$BUILD_DIR/examples/$EXAMPLE.uf2"
if [ ! -f "$UF2_FILE" ]; then
    echo -e "${RED}Error: File not found: $UF2_FILE${NC}"
    echo "Did you build the example first?"
    echo "Run: tools/build.sh -p rp2350"
    exit 1
fi

# Auto-detect mount point if not specified
if [ -z "$MOUNT_POINT" ]; then
    echo -e "${BLUE}Auto-detecting Pico mount point...${NC}"
    
    # Try common mount points
    for mp in /media/$USER/RPI-RP2 /media/RPI-RP2 /Volumes/RPI-RP2; do
        if [ -d "$mp" ]; then
            MOUNT_POINT="$mp"
            break
        fi
    done
    
    if [ -z "$MOUNT_POINT" ]; then
        echo -e "${YELLOW}Warning: Could not auto-detect Pico mount point${NC}"
        echo "Please ensure:"
        echo "  1. Pico 2 is connected via USB"
        echo "  2. BOOTSEL button was held during connection"
        echo "  3. RPI-RP2 drive is mounted"
        echo ""
        echo "Specify mount point manually with -m option"
        exit 1
    fi
fi

# Validate mount point
if [ ! -d "$MOUNT_POINT" ]; then
    echo -e "${RED}Error: Mount point not found: $MOUNT_POINT${NC}"
    echo "Is the Pico connected in bootloader mode?"
    exit 1
fi

# Flash
echo -e "${BLUE}Flashing $EXAMPLE to Pico 2...${NC}"
echo "Source:      $UF2_FILE"
echo "Destination: $MOUNT_POINT"
echo ""

cp "$UF2_FILE" "$MOUNT_POINT/"

echo -e "${GREEN}Flash successful!${NC}"
echo ""
echo "The Pico will now reboot and run the example."
echo "Connect to serial port (115200 baud) to see output:"
echo "  screen /dev/ttyACM0 115200"
echo "  or"
echo "  minicom -D /dev/ttyACM0 -b 115200"
