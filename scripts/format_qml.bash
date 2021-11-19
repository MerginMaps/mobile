#!/bin/bash

echo "QML STYLE CHECK STARTED"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PWD=`pwd`
cd $DIR

# We are currently using qmlfmt program available from homebrew on mac.
# It is available here: https://github.com/jesperhh/qmlfmt
# Ideally we should be using qmlformat tool from Qt, however due to a lack
# of their functionality (and bugs) we moved to qmlfmt.
# See https://bugreports.qt.io/browse/QTBUG-98422 - if this bug is fixed and
# released, we can move back to Qt's qmlformat.

# Code that is commented out is a leftover from using qmlformat, it is kept
# to be used in future

# <QMLFORMAT CODE>
# FORMATTER=""
#
# if [ $# -eq 0 ]; then
#     # No arguments provided, need to look for qmlformat in path
#     which qmlformat
#
#     if [ $? -ne 0 ]; then
#     	echo "[!] qmlformat not found in PATH" >&2
#     	exit 1
#     fi
#
#     FORMATTER=`which qmlformat`
# else
#     FORMATTER=`which $1`
#
#     if [ $? -ne 0 ]; then
#       echo "[!] passed qmlformat is invalid" >&2
#       exit 1
#     fi
# fi
#
# ${FORMATTER} -v | grep -q "6."
#
# if [ $? -ne 0 ]; then
#   echo "[!] qmlformat needs to be from Qt version 6.2 or higher! "
#   exit 1
# fi
# </QMLFORMAT CODE>

FORMATTER=`which qmlfmt`

if [ $? -ne 0 ]; then
  echo "[!] qmlfmt program not found! Path: ${PATH}"
  exit 1
fi

echo "Starting to format QML files"

RETURN=0

FILES=`find ../app ../qgsquick -name \*.qml* -print`

# <QMLFORMAT CODE>
# --indent-width 2: use 2 spaces to indent
# --normalize: reorder properties, includes and childs in components
#OPTIONS="--indent-width 2 --normalize"
# </QMLFORMAT CODE>

OPTIONS="-i 2"

for FILE in $FILES; do
    if [[ $FILE =~ \.qml$ ]]; then
        $FORMATTER $OPTIONS $FILE > $FILE.tmp
        cmp -s $FILE $FILE.tmp
        if [ $? -ne 0 ]; then
            echo "Changed $FILE" >&2
            RETURN=1
            diff -u $FILE $FILE.tmp >&2
            mv $FILE.tmp $FILE
        else
            echo "Unchanged $FILE" >&2
            rm $FILE.tmp
        fi
    else
       echo "Skipping $FILE" >&2
    fi
done

echo "Done checking QML files"

cd $PWD

exit $RETURN
