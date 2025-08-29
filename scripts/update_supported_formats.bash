#!/usr/bin/env bash
set -euo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

TOOLS_DIR="$PROJECT_DIR/build/vcpkg_installed/x64-linux/tools/gdal"

# Find the tools directory dynamically in (x64-linux | x64-osx | arm64-osx)
TOOLS_DIR=$(find "$PROJECT_DIR/build/vcpkg_installed" -maxdepth 2 -type d \
    \( -name "x64-linux" -o -name "x64-osx" -o -name "arm64-osx" \) \
    -exec echo {}/tools/gdal \; | head -n 1)

if [[ -z "$TOOLS_DIR" ]]; then
    echo "Error: Could not find tools/gdal directory under build/vcpkg_installed."
    exit 1
fi

GDALINFO="$TOOLS_DIR/gdalinfo"
OGRINFO="$TOOLS_DIR/ogrinfo"
OUTPUT_FILE="$PROJECT_DIR/docs/supported_formats.txt"

# Function to check if a command exists
check_command() {
    local cmd="$1"
    if [[ ! -x "$cmd" ]]; then
        echo "Error: $cmd not found or not executable."
        echo "Make sure you are building GDAL on linux."
        exit 1
    fi
}

check_command "$GDALINFO"
check_command "$OGRINFO"

{
    echo "===== GDAL Formats ====="
    "$GDALINFO" --formats
    echo
    echo "===== OGR Formats ====="
    "$OGRINFO" --formats
} > "$OUTPUT_FILE"

$PROJECT_DIR/build/app/Input --generate_QGIS_formats

echo "Formats info saved to $OUTPUT_FILE"
