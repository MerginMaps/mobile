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

# win: scripts/version.cmd
WIN_FILE=$DIR/version.cmd
echo "patching $WIN_FILE"
sed -i.orig -E "s|VERSIONMAJOR=[0-9]+|VERSIONMAJOR=$MAJOR|g" $WIN_FILE
sed -i.orig -E "s|VERSIONMINOR=[0-9]+|VERSIONMINOR=$MINOR|g" $WIN_FILE
sed -i.orig -E "s|VERSIONBUILD=[0-9]+|VERSIONBUILD=$BUILD|g" $WIN_FILE
rm -f $WIN_FILE.orig

echo "patching done"
