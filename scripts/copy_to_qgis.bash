#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "copy_to_qgis.bash QGIS_SRC_DIR"

QGIS_SRC_DIR=$1
QGIS_SRC_DIR=`realpath $QGIS_SRC_DIR`
QGSQUICK_QGIS_DIR="$QGIS_SRC_DIR/src/quickgui"
QGSQUICK_QGIS_TEST_DIR="$QGIS_SRC_DIR/tests/src/quickgui"
QGSQUICK_QGIS_TEST_HEADER="$QGIS_SRC_DIR/src/test/qgstest.h"
if [ ! -f "$QGSQUICK_QGIS_DIR/qgsquickutils.cpp" ]; then
  echo "Wrong QGIS_SRC_DIR, missing $QGSQUICK_QGIS_DIR/qgsquickutils.cpp!"
  exit 1;
fi

QGSQUICK_INPUT_DIR_ROOT=$DIR/../qgsquick/from_qgis
QGSQUICK_INPUT_DIR=$QGSQUICK_INPUT_DIR_ROOT/quickgui
QGSQUICK_INPUT_TEST_DIR=$QGSQUICK_INPUT_DIR_ROOT/tests
QGSQUICK_INPUT_TEST_HEADER=$QGSQUICK_INPUT_DIR_ROOT/qgstest.h
if [ ! -d "$QGSQUICK_INPUT_DIR" ]; then
  echo "Wrong QGSQUICK_INPUT_DIR"
  exit 1;
fi


# COPY
echo "update qgsquick"
rm -rf $QGSQUICK_QGIS_DIR/*
rsync -zavh --exclude="*.prepare" --exclude="*.sortinc" $QGSQUICK_INPUT_DIR/ $QGSQUICK_QGIS_DIR

echo "update tests"
rm -rf $QGSQUICK_QGIS_TEST_DIR/*
rsync -zavh --exclude="*.prepare" --exclude="*.sortinc" $QGSQUICK_INPUT_TEST_DIR/ $QGSQUICK_QGIS_TEST_DIR

echo "get test data"
rm -rf $QGSQUICK_QGIS_TEST_DATA_DIR/*
cp $QGSQUICK_INPUT_TEST_DATA_DIR/quickapp_project.qgs $QGSQUICK_QGIS_TEST_DATA_DIR/
cp $QGSQUICK_INPUT_TEST_DATA_DIR/points.* $QGSQUICK_QGIS_TEST_DATA_DIR/
cp $QGSQUICK_INPUT_TEST_DATA_DIR/polys.* $QGSQUICK_QGIS_TEST_DATA_DIR/
cp $QGSQUICK_INPUT_TEST_DATA_DIR/lines.* $QGSQUICK_QGIS_TEST_DATA_DIR/
cp $QGSQUICK_INPUT_TEST_DATA_DIR/zip/test.zip $QGSQUICK_QGIS_TEST_DATA_DIR/

echo "update qgis test framework"
rm -f $QGSQUICK_INPUT_TEST_HEADER
cp $QGSQUICK_INPUT_TEST_HEADER $QGSQUICK_QGIS_TEST_HEADER

# EXIT
cd $PWD
