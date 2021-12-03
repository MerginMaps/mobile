#!/bin/bash

BUILD_DIR=`pwd`/input
INSTALL_DIR=${BUILD_DIR}/out

set -e

#####
# PRINT ENV

echo "INSTALL_DIR: ${INSTALL_DIR}"
echo "SOURCE_DIR: ${SOURCE_DIR}"
echo "BUILD_DIR: ${BUILD_DIR}"
echo "ARCH: ${ARCH}"
echo "NDK: ${ANDROID_NDK_ROOT}"
echo "API: $ANDROIDAPI"

mkdir -p ${BUILD_DIR}/.gradle

pushd ${BUILD_DIR}

${QT_BASE}/bin/qmake -spec android-clang ANDROID_ABIS="${ARCH}" ${SOURCE_DIR}/app/input.pro
ls ${ANDROID_NDK_ROOT}/prebuilt/
${ANDROID_NDK_ROOT}/prebuilt/${ANDROID_NDK_HOST}/bin/make qmake_all
make -j ${CORES}

make install INSTALL_ROOT=${INSTALL_DIR}

if [ -f ${SOURCE_DIR}/Input_keystore.keystore ]; then
    echo "building release"
    ${QT_BASE}/bin/androiddeployqt \
        --sign ${SOURCE_DIR}/Input_keystore.keystore input \
        --storepass ${INPUTKEYSTORE_STOREPASS} \
        --keypass ${INPUTKEYSTORE_STOREPASS} \
        --input ${BUILD_DIR}/android-Input-deployment-settings.json \
        --output ${INSTALL_DIR} \
        --aab \
        --deployment bundled \
        --gradle
else
    echo "missing certificate! exit"
    exit 1
fi
