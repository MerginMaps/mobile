#!/usr/bin/env bash

# see https://doc.qt.io/qt-5/linguist-manager.html

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PWD=`pwd`
LUPDATE_PARAMS="-extensions qml,cpp,hpp,h,ui,c -ts"

echo "update_18n_ts.bash QGIS_SRC_DIR QT_DIST_DIR"

QGIS_SRC_DIR=$1
QGIS_SRC_DIR=`realpath $QGIS_SRC_DIR`
if [ ! -f "$QGIS_SRC_DIR/src/quickgui/qgsquickutils.cpp" ]; then
  echo "Wrong QGIS_SRC_DIR, missing $QGIS_SRC_DIR/src/quickgui/qgsquickutils.cpp !"
  exit 1;
fi

QT_DIR=$2
QT_DIR=`realpath $QT_DIR`
if [ ! -f "$QT_DIR/translations/qt_fr.qm" ]; then
  echo "Wrong QT_DIR, missing $QT_DIR/translations/qt_fr.qm !"
  exit 1;
fi
export OLD_PATH=$PATH
export PATH=$QT_DIR/bin:$PATH

I18N_DIR=$DIR/../app/i18n
cd $I18N_DIR
INPUT_TS="$INPUT_TS ./input_en.ts"
QUICKQUI_DIR=`realpath --relative-to=$I18N_DIR $QGIS_SRC_DIR/src/quickgui`
INPUT_DIR=../

lupdate -noobsolete $INPUT_DIR $QUICKQUI_DIR $LUPDATE_PARAMS $INPUT_TS

export PATH=$OLD_PATH
echo "update i18n done"
cd $PWD