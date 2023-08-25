#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PWD=`pwd`
cd $DIR

./format_cmake.bash
./format_cpp.bash

cd $PWD
