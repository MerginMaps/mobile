#!/usr/bin/env bash
set -e
# see https://doc.qt.io/qt-5/linguist-manager.html

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PWD=`pwd`
LUPDATE_PARAMS="-extensions qml,cpp,hpp,h,ui,c -ts"

echo "update_18n_ts.bash QT_DIST_DIR"

if [ ! -f "$DIR/../qgsquick/qgsquickmapcanvasmap.cpp" ]; then
  echo "Missing $DIR/../qgsquick/qgsquickmapcanvasmap.cpp!"
  exit 1;
fi

QT_DIR=$1
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
QUICKQUI_DIR=`realpath --relative-to=$I18N_DIR $DIR/../qgsquick`
INPUT_DIR=../

lupdate -noobsolete $INPUT_DIR $QUICKQUI_DIR $LUPDATE_PARAMS $INPUT_TS

export PATH=$OLD_PATH
echo "update i18n done"
cd $PWD
