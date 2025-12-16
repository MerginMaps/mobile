#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PWD=`pwd`
cd $DIR

# see https://cmake-format.readthedocs.io/en/latest/configuration.html
CONFIG=$DIR/cmake_format_config.py

RETURN=0
FORMATTER=$(which cmake-format)
if [ $? -ne 0 ]; then
	echo "[!] cmake-format not installed." >&2
    echo "pip3 install cmakelang"
	exit 1
fi

FILES=`find .. -name \*.cmake\* -print -o -name \CMakeLists.txt -print | grep -v build`

for FILE in $FILES; do
    cmake-format -c cmake_format_config.py --check $FILE
    if [ $? -ne 0 ]; then  
      cp $FILE $FILE.orig
      cmake-format -c cmake_format_config.py --in-place $FILE
      cmp -s $FILE.orig $FILE 
      if [ $? -ne 0 ]; then
        echo "Changed $FILE" >&2
        RETURN=1
        diff -u $FILE.orig $FILE >&2
      fi
      rm $FILE.orig
    else
      echo "Unchanged $FILE" >&2
    fi
done

exit $RETURN

cd $PWD
