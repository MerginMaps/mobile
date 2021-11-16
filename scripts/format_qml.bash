#!/bin/bash

# iterate over qml files and run qmlformat

echo "QML STYLE CHECK STARTED"

FORMATTER=""

if [ $# -eq 0 ]; then
    # No arguments provided, need to look for qmlformat in path
    env
    which qmlformat

    if [ $? -ne 0 ]; then
    	echo "[!] qmlformat not found in PATH" >&2
    	exit 1
    fi

    FORMATTER=`which qmlformat`
else
    FORMATTER=`which $1`

    if [ $? -ne 0 ]; then
      echo "[!] passed qmlformat is invalid" >&2
      exit 1
    fi
fi

${FORMATTER} -v | grep -q "6."

if [ $? -ne 0 ]; then
  echo "[!] qmlformat needs to be from Qt version 6.2 or higher! "
  exit 1
fi

echo "Starting to format QML files"

RETURN=0

FILES=`find ../app ../qgsquick -name "*.qml" | grep "editor" | grep "text"`

# --indent-width 2: use 2 spaces to indent
# --normalize: reorder properties, includes and childs in components
OPTIONS="--indent-width 2 --normalize"

for FILE in $FILES; do
    if [[ $FILE =~ \.qml$ ]]; then
        $FORMATTER $OPTIONS $FILE > $FILE.tmp
        cmp -s $FILE $FILE.tmp
        if [ $? -ne 0 ]; then
            echo "Changed $FILE" >&2
            RETURN=1
            mv $FILE.tmp $FILE
        else
            rm $FILE.tmp
            echo "Unchanged $FILE" >&2
        fi
    else
       echo "Skipping $FILE" >&2
    fi
done

exit $RETURN
