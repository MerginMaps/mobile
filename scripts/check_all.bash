#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PWD=`pwd`
cd $DIR
CI=0 ./astyle.bash `find .. -name \*.h* -print -o -name \*.c* -print`
cd $PWD
