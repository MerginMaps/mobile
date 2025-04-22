#!/usr/bin/env bash

set -e

# TODO fix!

echo "update_qt_version.bash MAJOR.MINOR.BUILD"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
VERSION=$1
VER_PARTS=(${VERSION//./ })
MAJOR=${VER_PARTS[0]}
MINOR=${VER_PARTS[1]}
BUILD=${VER_PARTS[2]}

echo "using QT version $MAJOR.$MINOR.$BUILD"

############
# CMakeLists
CMAKE_FILE=$DIR/../CMakeLists.txt
echo "patching $CMAKE_FILE"
# e.g. set(QT_VERSION_DEFAULT "6.5.2")
sed -i.orig -E "s|QT_VERSION_DEFAULT \"[0-9]+.[0-9]+.[0-9]+\"|QT_VERSION_DEFAULT \"${VERSION}\"|g" $CMAKE_FILE
rm -f $CMAKE_FILE.orig

############
# INSTALL.md
INSTALL_FILE=$DIR/../INSTALL.md
echo "patching $INSTALL_FILE"
# e.g. /Qt/6.5.2/ unix style
sed -i.orig -E "s|/Qt/[0-9]+.[0-9]+.[0-9]+/|/Qt/${VERSION}/|g" $INSTALL_FILE
# e.g. \Qt\6.5.2\ win style
sed -i.orig -E "s|\\\Qt\\\[0-9]+.[0-9]+.[0-9]+\\\|\\\Qt\\\\${VERSION}\\\|g" $INSTALL_FILE
rm -f $INSTALL_FILE.orig

############
# WORKFLOWS
for FNAME in \
    $DIR/../.github/workflows/android.yml \
    $DIR/../.github/workflows/code_style.yml \
    $DIR/../.github/workflows/gallery.yml \
    $DIR/../.github/workflows/i18n.yml \
    $DIR/../.github/workflows/ios.yml \
    $DIR/../.github/workflows/linux.yml \
    $DIR/../.github/workflows/macos.yml \
    $DIR/../.github/workflows/macos_arm64.yml \
    $DIR/../.github/workflows/win.yml
do
    echo "patching $FNAME"
    # e.g. QT_VERSION: '6.5.2'
    sed -i.orig -E "s|QT_VERSION: '[0-9]+.[0-9]+.[0-9]+'|QT_VERSION: '${VERSION}'|g" $FNAME
    rm -f $FNAME.orig
done

# DONE
echo "patching done"
