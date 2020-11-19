#!/bin/bash

# Script to build qgsquick placed in input/qgsquick
# 1. Duplicate this script (e.g. to have .user extension (due to .gitignore)) 
# 2. Set values of variables

STAGE_PATH=""		      	# path to input-sdk stage directory ( e.g. input-sdk/build/stage/arm64-v8a/" ) - also add architecture name at the end of path
QT_ANDROID=""		      	# path to Qt android ( e.g. Qt/<version>/android )
ANDROID_NDK_ROOT=""   		# path to used NDK

if [ -z ${ANDROID_NDK_ROOT} ]; then
	echo "Variable ${!ANDROID_NDK_ROOT@} not set"
  echo "Open script to see which variables must be set"
	exit -1
fi

if [ -z ${QT_ANDROID} ]; then
	echo "Variable ${!QT_ANDROID@} not set"
  echo "Open script to see which variables must be set"
	exit -1
fi

if [ -z ${STAGE_PATH} ]; then
	echo "Variable ${!STAGE_PATH@} not set"
  echo "Open script to see which variables must be set"
	exit -1
fi

SOURCE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}	" )/.." && pwd )"

ARCH="arm64-v8a" 	      	# target architecture ( arm64-v8a or armeabi-v7a )
ANDROID_API=23        		# Android API version

# Optionally change build directories
BUILD_DIR_QGSQUICK="${SOURCE_DIR}/build-qgsquick-android"
INSTALL_DIR_QGSQUICK="${BUILD_DIR_QGSQUICK}/out"

echo "SOURCE_DIR: ${SOURCE_DIR}"
echo "BUILD_DIR_QGSQUICK: ${BUILD_DIR_QGSQUICK}"
echo "INSTALL_DIR_QGSQUICK: ${INSTALL_DIR_QGSQUICK}"
echo "ARCH: ${ARCH}"
echo "NDK: ${ANDROID_NDK_ROOT}"
echo "API: ${ANDROIDAPI}"

echo "Creating build dir.."

mkdir -p ${BUILD_DIR_QGSQUICK}
pushd ${BUILD_DIR_QGSQUICK}


### CMAKE

echo "Starting CMAKE command.."

cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK_ROOT}/build/cmake/android.toolchain.cmake \
    -DCMAKE_FIND_ROOT_PATH:PATH="${ANDROID_NDK_ROOT};${QT_ANDROID};$STAGE_PATH" \
    -DANDROID_ABI=${ARCH} \
    -DANDROID_NDK=${ANDROID_NDK_ROOT} \
    -DANDROID_NATIVE_API_LEVEL=$ANDROIDAPI \
    -DANDROID=ON \
    -DANDROID_STL=c++_shared \
    -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR_QGSQUICK} \
    -DCMAKE_PREFIX_PATH=$QT_ANDROID \
    -DFORCE_STATIC_LIBS=FALSE \
    -DUSE_QGIS_BUILD_DIR=FALSE \
    -DENABLE_QT5=ON \
    -DQGIS_INSTALL_PATH=$STAGE_PATH \
    -DANDROID_INPUT_SDK_PATH=$STAGE_PATH \
    -DQGIS_CMAKE_PATH=${STAGE_PATH}/cmake \
  ${SOURCE_DIR}/qgsquick


### BUILD

CORES=`nproc --all`

echo "CMAKE done, running build with ${CORES} cores"

make -j${CORES} VERBOSE=1
make install INSTALL_ROOT=${INSTALL_DIR_QGSQUICK}

mkdir -p ${INSTALL_DIR_QGSQUICK}/images
cp ${SOURCE_DIR}/qgsquick/from_qgis/plugin/qgsquickplugin.h ${INSTALL_DIR_QGSQUICK}/include
cp -R ${SOURCE_DIR}/qgsquick/from_qgis/images ${INSTALL_DIR_QGSQUICK}/images/QgsQuick

