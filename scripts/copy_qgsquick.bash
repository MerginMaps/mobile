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
# TODO

# now force copy the files to local file
rm -rf $QGSQUICK_INPUT_DIR/*
cp -rf $QGSQUICK_QGIS_DIR/* $QGSQUICK_INPUT_DIR/

echo "update qgsquick"
cd $PWD