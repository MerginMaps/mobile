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

# ios: app/ios/Info.plist (JUST CFBundleShortVersionString, CFBundleVersion is calculated automatically)
IOS_FILE=$DIR/../app/ios/Info.plist
echo "patching $IOS_FILE"

sed -i.orig -E "s|<string>[0-9]+\\.[0-9]+\\.[0-9]+<\\/string>|<string>$VERSION<\\/string>|g" $IOS_FILE
rm -f $IOS_FILE.orig

# android: app/version.pri
ANDROID_FILE=$DIR/../app/version.pri
echo "patching $ANDROID_FILE"
sed -i.orig -E "s|VERSION_MAJOR = [0-9]+|VERSION_MAJOR = $MAJOR|g" $ANDROID_FILE
sed -i.orig -E "s|VERSION_MINOR = [0-9]+|VERSION_MINOR = $MINOR|g" $ANDROID_FILE
sed -i.orig -E "s|VERSION_FIX = [0-9]+|VERSION_FIX = $BUILD|g" $ANDROID_FILE
rm -f $ANDROID_FILE.orig

# .zenodo.json
ZENODO_FILE=$DIR/../.zenodo.json
echo "patching $ZENODO_FILE"
sed -i.orig -E "s|\"version\": \"[0-9]+\.[0-9]+\.[0-9]+\"|\"version\": \"$MAJOR\.$MINOR\.$BUILD\"|g" $ZENODO_FILE
sed -i.orig -E "s|https://github.com/MerginMaps/input/tree/[0-9]+\.[0-9]+\.[0-9]|https://github.com/MerginMaps/input/tree/$MAJOR\.$MINOR\.$BUILD|g" $ZENODO_FILE
rm -f $ZENODO_FILE.orig

# CITATION.cff
CITATION_FILE=$DIR/../CITATION.cff
echo "patching $CITATION_FILE"
sed -i.orig -E "s|cff-version: [0-9]+\.[0-9]+\.[0-9]+|cff-version: $MAJOR\.$MINOR\.$BUILD|g" $CITATION_FILE
rm -f $CITATION_FILE.orig

echo "patching done"
