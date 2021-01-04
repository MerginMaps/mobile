#!/bin/bash

# This is intended to be run within a lutraconsulting/input-sdk docker container.

SOURCE_DIR=/usr/src/input
if [[ -z ${BUILD_FOLDER+x} ]]; then
    BUILD_DIR=${SOURCE_DIR}/build-docker
    BUILD_DIR_QGSQUICK=${SOURCE_DIR}/build-docker-quick
else
    BUILD_DIR=${SOURCE_DIR}/${BUILD_FOLDER}
    BUILD_DIR_QGSQUICK=${SOURCE_DIR}/${BUILD_FOLDER}-quick
fi
if [[ -z ${ARCH+x} ]]; then
    ARCH=armeabi-v7a
fi
INSTALL_DIR=${BUILD_DIR}/out
INSTALL_DIR_QGSQUICK=${BUILD_DIR_QGSQUICK}/out
QT_ANDROID=${QT_ANDROID_BASE}/android
CORES=$(cat /proc/cpuinfo | grep processor | wc -l)
STAGE_PATH=/home/input-sdk/${ARCH}

set -e

# Replace the version number in version.pri with the one from the VERSION which is being built
if [[ -n ${VERSION} ]];
then
  echo "Building release version ${VERSION}"
  sed -i "s/VERSION_MAJOR\s*= .*/VERSION_MAJOR = $(echo "${VERSION}" | cut -f 2 -d '-' | cut -f 1 -d '.')/g" ${SOURCE_DIR}/app/version.pri
  sed -i "s/VERSION_MINOR\s*= .*/VERSION_MINOR = $(echo "${VERSION}" | cut -f 2 -d '.')/g" ${SOURCE_DIR}/app/version.pri
  sed -i "s/VERSION_FIX\s*= .*/VERSION_FIX = $(echo "${VERSION}" | cut -f 3 -d '.')/g" ${SOURCE_DIR}/app/version.pri
  grep 'VERSION_MAJOR' ${SOURCE_DIR}/app/version.pri
  grep 'VERSION_MINOR' ${SOURCE_DIR}/app/version.pri
  grep 'VERSION_FIX' ${SOURCE_DIR}/app/version.pri
fi

# TODO take from input-sdk?
export ANDROIDAPI=23
if [ "X${ARCH}" == "Xarmeabi-v7a" ]; then
  export TOOLCHAIN_SHORT_PREFIX=arm-linux-androideabi
  export TOOLCHAIN_PREFIX=arm-linux-androideabi
  export QT_ARCH_PREFIX=armv7
elif [ "X${ARCH}" == "Xarm64-v8a" ]; then
  export TOOLCHAIN_SHORT_PREFIX=aarch64-linux-android
  export TOOLCHAIN_PREFIX=aarch64-linux-android
  export QT_ARCH_PREFIX=arm64 # watch out when changing this, openssl depends on it
else
  echo "Error: Please report issue to enable support for arch (${ARCH})."
  exit 1
fi

#####
# PRINT ENV

echo "INSTALL_DIR: ${INSTALL_DIR}"
echo "INSTALL_DIR_QGSQUICK: ${INSTALL_DIR_QGSQUICK}"
echo "BUILD_DIR: ${BUILD_DIR}"
echo "BUILD_DIR_QGSQUICK: ${BUILD_DIR_QGSQUICK}"
echo "ARCH: ${ARCH}"
echo "CORES ${CORES}"
echo "NDK: ${ANDROID_NDK_ROOT}"
echo "API: $ANDROIDAPI"

######################
# QGS QUICK
mkdir -p ${BUILD_DIR_QGSQUICK}
pushd ${BUILD_DIR_QGSQUICK}

export ANDROID_NDK=$ANDROID_NDK_ROOT

cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DANDROID_LINKER_FLAGS=$ANDROID_CMAKE_LINKER_FLAGS \
    -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK_ROOT}/build/cmake/android.toolchain.cmake \
    -DCMAKE_CXX_FLAGS_RELEASE=-g0 \
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

make -j ${CORES} VERBOSE=1
make install INSTALL_ROOT=${INSTALL_DIR_QGSQUICK}

mkdir -p ${INSTALL_DIR_QGSQUICK}/images
cp ${SOURCE_DIR}/qgsquick/from_qgis/plugin/qgsquickplugin.h ${INSTALL_DIR_QGSQUICK}/include
cp -R ${SOURCE_DIR}/qgsquick/from_qgis/images ${INSTALL_DIR_QGSQUICK}/images/QgsQuick
######################
# Input

# see https://bugreports.qt.io/browse/QTBUG-80756
export ANDROID_TARGET_ARCH=${ARCH}

mkdir -p ${BUILD_DIR}/.gradle
# androiddeployqt needs gradle and downloads it to /root/.gradle. By linking it to the build folder, this will be cached between builds.
ln -s ${BUILD_DIR}/.gradle /root/.gradle

pushd ${BUILD_DIR}
cp ${SOURCE_DIR}/scripts/ci/config.pri ${SOURCE_DIR}/app/config.pri
${QT_ANDROID}/bin/qmake -spec android-clang ANDROID_ABIS="${ARCH}" ${SOURCE_DIR}/app/input.pro
${ANDROID_NDK_ROOT}/prebuilt/${ANDROID_NDK_HOST}/bin/make qmake_all
make -j ${CORES}
make install INSTALL_ROOT=${INSTALL_DIR}

if [ -f ${SOURCE_DIR}/Input_keystore.keystore ]; then
    ${QT_ANDROID}/bin/androiddeployqt \
	    --sign ${SOURCE_DIR}/Input_keystore.keystore input \
	    --storepass ${INPUTKEYSTORE_STOREPASS} \
	    --keypass ${INPUTKEYSTORE_STOREPASS} \
      --input ${BUILD_DIR}/android-Input-deployment-settings.json \
	    --output ${INSTALL_DIR} \
	    --deployment bundled \
	    --gradle
else
    ${QT_ANDROID}/bin/androiddeployqt \
      --input ${BUILD_DIR}/android-Input-deployment-settings.json \
	    --output ${INSTALL_DIR} \
	    --deployment bundled \
	    --gradle
fi

chown -R $(stat -c "%u" .):$(stat -c "%u" .) .
popd
