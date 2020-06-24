#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PWD=`pwd`

echo "release_18n_ts.bash"

# i18n qt resource file
QRC_FILE=$DIR/../app/i18n/input_i18n.qrc
echo "<RCC>" > $QRC_FILE
echo "  <qresource prefix=\"/\"> " >> $QRC_FILE

cd $DIR/../app/i18n
FILES=`find . -type f -name "*.ts"`
for i in $FILES
do
    QMFILE=${i/.ts/.qm}
    lrelease $i -qm ${QMFILE}
done

FILES_QT=`find . -type f -name "*.qm"`
for i in $FILES_QT
do
    echo "    <file>${i/.\//}</file> " >> $QRC_FILE
done

echo "  </qresource> " >> $QRC_FILE
echo "</RCC>" >> $QRC_FILE

cd $PWD

echo "release i18n done"

cd $PWD