#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "copy_qgsquick.bash QGIS_SRC_DIR"

QGIS_SRC_DIR=$1
QGIS_SRC_DIR=`realpath $QGIS_SRC_DIR`
QGSQUICK_QGIS_DIR="$QGIS_SRC_DIR/src/quickgui"
QGSQUICK_QGIS_TEST_DIR="$QGIS_SRC_DIR/tests/src/quickgui"
QGSQUICK_QGIS_TEST_HEADER="$QGIS_SRC_DIR/src/test/qgstest.h"
QGSQUICK_QGIS_TEST_DATA_DIR="$QGIS_SRC_DIR/tests/testdata"
if [ ! -f "$QGSQUICK_QGIS_DIR/qgsquickutils.cpp" ]; then
  echo "Wrong QGIS_SRC_DIR, missing $QGSQUICK_QGIS_DIR/qgsquickutils.cpp!"
  exit 1;
fi

QGSQUICK_INPUT_DIR_ROOT=$DIR/../qgsquick/from_qgis
QGSQUICK_INPUT_DIR=$QGSQUICK_INPUT_DIR_ROOT/quickgui
QGSQUICK_INPUT_TEST_DIR=$QGSQUICK_INPUT_DIR_ROOT/tests/quickgui
QGSQUICK_INPUT_TEST_HEADER=$QGSQUICK_INPUT_DIR_ROOT/qgstest.h
QGSQUICK_INPUT_TEST_DATA_DIR=$QGSQUICK_INPUT_DIR_ROOT/testdata
if [ ! -d "$QGSQUICK_INPUT_DIR_ROOT" ]; then
  echo "Wrong QGSQUICK_INPUT_DIR"
  exit 1;
fi

######### DIFFERS?
echo "differs QUICKGUI??"
diff -r $QGSQUICK_INPUT_DIR $QGSQUICK_QGIS_DIR
DIFFERS1=$?
echo "The QGIS and INPUT copy QUICKGUI differs? $DIFFERS1"

echo "QUICKTEST differs??"
diff -r $QGSQUICK_INPUT_TEST_DIR $QGSQUICK_QGIS_TEST_DIR
DIFFERS2=$?
echo "The QGIS and INPUT copy QUICKTEST differs? $DIFFERS2"

######### FORCE update
echo "update qgsquick"
rm -rf $QGSQUICK_INPUT_DIR/*
rsync -zavh --exclude="*.prepare" --exclude="*.sortinc" $QGSQUICK_QGIS_DIR/ $QGSQUICK_INPUT_DIR

echo "update tests"
rm -rf $QGSQUICK_INPUT_TEST_DIR/*
rsync -zavh --exclude="*.prepare" --exclude="*.sortinc" $QGSQUICK_QGIS_TEST_DIR/ $QGSQUICK_INPUT_TEST_DIR

echo "get test data"
rm -rf $QGSQUICK_INPUT_TEST_DATA_DIR/*
cp $QGSQUICK_QGIS_TEST_DATA_DIR/quickapp_project.qgs $QGSQUICK_INPUT_TEST_DATA_DIR/
cp $QGSQUICK_QGIS_TEST_DATA_DIR/points.* $QGSQUICK_INPUT_TEST_DATA_DIR/
cp $QGSQUICK_QGIS_TEST_DATA_DIR/polys.* $QGSQUICK_INPUT_TEST_DATA_DIR/
cp $QGSQUICK_QGIS_TEST_DATA_DIR/lines.* $QGSQUICK_INPUT_TEST_DATA_DIR/
cp $QGSQUICK_QGIS_TEST_DATA_DIR/zip/test.zip $QGSQUICK_INPUT_TEST_DATA_DIR/

echo "update qgis test framework"
rm -f $QGSQUICK_INPUT_TEST_HEADER
cp $QGSQUICK_QGIS_TEST_HEADER $QGSQUICK_INPUT_TEST_HEADER

######### EXIT
cd $PWD
DIFFERS=$((DIFFERS1+DIFFERS2))
echo "The QGIS and INPUT copy differs? $DIFFERS"
exit $DIFFERS