#!/usr/bin/env bash
set -euo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# Find the tools directory dynamically in (x64-linux | x64-osx | arm64-osx)
# within any build directory of the project (*build*/vcpkg_installed/<triplet>/tools/gdal)
TOOLS_DIR=""
for dir in "$PROJECT_DIR"/*build*/vcpkg_installed/{x64-linux,x64-osx,arm64-osx}/tools/gdal; do
    if [[ -d "$dir" ]]; then
        TOOLS_DIR="$dir"
        BUILD_DIR="$(cd "$dir/../../../.." && pwd)"
        break
    fi
done

if [[ -z "$TOOLS_DIR" ]]; then
    echo "Error: Could not find tools/gdal directory under any *build*/vcpkg_installed in $PROJECT_DIR."
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

# On macOS the binary is inside the app bundle
if [[ -x "$BUILD_DIR/app/MerginMaps.app/Contents/MacOS/MerginMaps" ]]; then
    MM_APP="$BUILD_DIR/app/MerginMaps.app/Contents/MacOS/MerginMaps"
else
    MM_APP="$BUILD_DIR/app/MerginMaps"
fi
check_command "$MM_APP"

"$MM_APP" --generate_QGIS_formats

echo "Formats info saved to $OUTPUT_FILE"
