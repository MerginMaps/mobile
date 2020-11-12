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
if [ ! -d "$QGSQUICK_INPUT_DIR" ]; then
  echo "Wrong QGSQUICK_INPUT_DIR"
  exit 1;
fi

# first see how many files differs
echo "differs??"
diff qgsquick/ ../QGIS/src/quickgui/
DIFFERS=$?
echo "The QGIS and INPUT copy differs? $DIFFERS"

# now force copy the files to local file
echo "update qgsquick"
rm -rf $QGSQUICK_INPUT_DIR/*
rsync -zavh --exclude="*.prepare" --exclude="*.sortinc" ../QGIS/src/quickgui/ qgsquick/

cd $PWD
echo "The QGIS and INPUT copy differs? $DIFFERS"
exit $DIFFERS