#!/bin/bash

BUILD_DIR=`pwd`/input
INSTALL_DIR=${BUILD_DIR}/out

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
