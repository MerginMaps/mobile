#!/bin/bash

# Script formats both cpp and qml files - intended to be used locally.

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PWD=`pwd`
cd $DIR

./format_cpp.bash

./format_qml.bash

cd $PWD
