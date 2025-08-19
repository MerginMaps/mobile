#!/usr/bin/env bash

set -e

echo "update_all_versions.bash MAJOR.MINOR.BUILD"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
VERSION=$1
VER_PARTS=(${VERSION//./ })
MAJOR=${VER_PARTS[0]}
MINOR=${VER_PARTS[1]}
BUILD=${VER_PARTS[2]}

echo "using version $MAJOR.$MINOR.$BUILD"

# CMakeLists.txt
CMAKE_FILE=$DIR/../CMakeLists.txt
echo "patching $CMAKE_FILE"
sed -i.orig -E "s|MM_VERSION_MAJOR \"[0-9]+\"|MM_VERSION_MAJOR \"$MAJOR\"|g" $CMAKE_FILE
sed -i.orig -E "s|MM_VERSION_MINOR \"[0-9]+\"|MM_VERSION_MINOR \"$MINOR\"|g" $CMAKE_FILE
sed -i.orig -E "s|MM_VERSION_PATCH \"[0-9]+\"|MM_VERSION_PATCH \"$BUILD\"|g" $CMAKE_FILE
rm -f $CMAKE_FILE.orig

# .zenodo.json
ZENODO_FILE=$DIR/../.zenodo.json
echo "patching $ZENODO_FILE"
sed -i.orig -E "s|\"version\": \"[0-9]+\.[0-9]+\.[0-9]+\"|\"version\": \"$MAJOR\.$MINOR\.$BUILD\"|g" $ZENODO_FILE
sed -i.orig -E "s|https://github.com/MerginMaps/mobile/tree/[0-9]+\.[0-9]+\.[0-9]|https://github.com/MerginMaps/mobile/tree/$MAJOR\.$MINOR\.$BUILD|g" $ZENODO_FILE
rm -f $ZENODO_FILE.orig

# CITATION.cff
CITATION_FILE=$DIR/../CITATION.cff
echo "patching $CITATION_FILE"
sed -i.orig -E "s|cff-version: [0-9]+\.[0-9]+\.[0-9]+|cff-version: $MAJOR\.$MINOR\.$BUILD|g" $CITATION_FILE
rm -f $CITATION_FILE.orig

# vcpkg.json
VCPKG_FILE=$DIR/../vcpkg.json
echo "patching $VCPKG_FILE"
sed -i.orig -E "s|\"version\": \"[0-9]+\.[0-9]+\.[0-9]+\"|\"version\": \"$MAJOR\.$MINOR\.$BUILD\"|g" $VCPKG_FILE
rm -f $VCPKG_FILE.orig

echo "patching done"