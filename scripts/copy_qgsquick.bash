#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "copy_qgsquick.bash QGIS_SRC_DIR"

QGIS_SRC_DIR=$1
QGIS_SRC_DIR=`realpath $QGIS_SRC_DIR`
QGSQUICK_QGIS_DIR="$QGIS_SRC_DIR/src/quickgui"
if [ ! -f "$QGSQUICK_QGIS_DIR/qgsquickutils.cpp" ]; then
  echo "Wrong QGIS_SRC_DIR, missing $QGSQUICK_QGIS_DIR/qgsquickutils.cpp!"
  exit 1;
fi

QGSQUICK_INPUT_DIR=$DIR/../qgsquick
if [ ! -f "$QGSQUICK_INPUT_DIR/qgsquickutils.cpp" ]; then
  echo "Wrong QGSQUICK_INPUT_DIR"
  exit 1;
fi

######### DIFFERS?
echo "differs QUICKGUI??"
diff -r \
      --exclude="*.prepare" \
      --exclude="*.sortinc" \
      --exclude="CMakeLists.txt" \
      --exclude="qgsquickplugin.*" \
      --exclude="qmldir" \
      --exclude="qgsquick.qrc" \
      --exclude="*.qrc" \
      --exclude="*.pri" \
      --exclude="qgis_quick.h" \
    $QGSQUICK_INPUT_DIR $QGSQUICK_QGIS_DIR

DIFFERS=$?
echo "The QGIS and INPUT copy QUICKGUI differs? $DIFFERS"

######### FORCE update
echo "update qgsquick"
rm -rf $QGSQUICK_INPUT_DIR/*
rsync -zavh \
      --exclude="*.prepare" \
      --exclude="*.sortinc" \
      --exclude="CMakeLists.txt" \
      --exclude="qgsquickplugin.*" \
      --exclude="qmldir" \
      --exclude="*.qrc" \
      --exclude="*.pri" \
      --exclude="qgis_quick.h" \
      $QGSQUICK_QGIS_DIR/ $QGSQUICK_INPUT_DIR

######### EXIT
cd $PWD
echo "The QGIS and INPUT copy differs? $DIFFERS"
exit $DIFFERS