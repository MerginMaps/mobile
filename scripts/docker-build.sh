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
INSTALL_DIR_QGSQUICK=${BUILD_DIR}/out-quick
QT_ANDROID=${QT_ANDROID_BASE}/android

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

echo "INSTALL_DIR: ${INSTALL_DIR}"
echo "INSTALL_DIR_QGSQUICK: ${INSTALL_DIR_QGSQUICK}"
echo "BUILD_DIR: ${BUILD_DIR}"
echo "BUILD_DIR_QGSQUICK: ${BUILD_DIR_QGSQUICK}"
echo "ARCH: ${ARCH}"
echo "API: $ANDROIDAPI"

######################
# QGS QUICK
mkdir -p ${BUILD_DIR_QGSQUICK}
pushd ${BUILD_DIR_QGSQUICK}

# -DANDROID_LINKER_FLAGS=$ANDROID_CMAKE_LINKER_FLAGS \
cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK_ROOT}/build/cmake/android.toolchain.cmake \
  -DCMAKE_CXX_FLAGS_RELEASE=-g0 \
  -DCMAKE_FIND_ROOT_PATH:PATH=${ANDROID_NDK_ROOT};${QT_ANDROID};/home/input-sdk/${ARCH} \
  -DANDROID_ABI=${ARCH} -DANDROID_NDK=${ANDROID_NDK_ROOT} \
  -DANDROID_NATIVE_API_LEVEL=$ANDROIDAPI \
  -DANDROID=ON \
  -DANDROID_STL=c++_shared \

  ${SOURCE_DIR}/qgsquick

make
make install INSTALL_ROOT=${INSTALL_DIR_QGSQUICK}

mkdir -p ${INSTALL_DIR_QGSQUICK}/images
cp ${QGSQUICK_DIR}/from_qgis/plugin/qgsquickplugin.h ${INSTALL_DIR_QGSQUICK}/include
cp -R ${QGSQUICK_DIR}/from_qgis/images ${INSTALL_DIR_QGSQUICK}/images/QgsQuick
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
make
make install INSTALL_ROOT=${INSTALL_DIR}

if [ -f ${SOURCE_DIR}/Input_keystore.keystore ]; then
    ${QT_ANDROID}/bin/androiddeployqt \
	    --sign ${SOURCE_DIR}/Input_keystore.keystore input \
	    --storepass ${STOREPASS} \
	    --keypass ${STOREPASS} \
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
