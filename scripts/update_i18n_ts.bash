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

LANGS="fr es sk"

I18N_DIR=$DIR/../app/i18n
cd $I18N_DIR

# Input APP
INPUT_TS=
for i in $LANGS
do
    INPUT_TS="$INPUT_TS ./input_$i.ts"
done

QUICKQUI_DIR=`realpath --relative-to=$I18N_DIR $QGIS_SRC_DIR/src/quickgui`
INPUT_DIR=../

lupdate $INPUT_DIR $QUICKQUI_DIR $LUPDATE_PARAMS $INPUT_TS

# QT
# for i in $LANGS
# do
#    cp -v $QT_DIR/translations/qt_$i.qm $DIR/../app/i18n/
# done

echo "update i18n done"
cd $PWD