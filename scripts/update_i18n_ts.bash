#!/usr/bin/env bash
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PWD=`pwd`
LUPDATE_PARAMS="-extensions qml,cpp,hpp,h,ui,c -ts"

echo "update_18n_ts.bash QT_DIST_DIR"

QT_DIR=$1
QT_DIR=`realpath $QT_DIR`

if [ ! -f "$QT_DIST_DIR/bin/lupdate" ]; then
  echo "Wrong QT_DIR, missing $QT_DIST_DIR/bin/lupdate!"
  exit 1;
fi

if [ ! -f "$QT_DIR/translations/qt_fr.qm" ]; then
  echo "Wrong QT_DIR, missing $QT_DIR/translations/qt_fr.qm !"
  exit 1;
fi

I18N_DIR=$DIR/../app/i18n
cd $I18N_DIR
INPUT_TS="$INPUT_TS ./input_en.ts"
INPUT_DIR=../

$QT_DIST_DIR/bin/lupdate -noobsolete $INPUT_DIR $LUPDATE_PARAMS $INPUT_TS

echo "update i18n done"
cd $PWD
