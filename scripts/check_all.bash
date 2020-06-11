#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PWD=`pwd`
cd $DIR
./astyle.bash `find ../app -name \*.mm* -print -o -name \*.h* -print -o -name \*.c* -print`
cd $PWD
