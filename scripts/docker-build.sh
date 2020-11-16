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
CORES=$(cat /proc/cpuinfo | grep processor | wc -l)

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
echo "CORES ${CORES}"

# TODO take from input-sdk?
export ANDROIDAPI=23
echo "API: $ANDROIDAPI"

######################
# QGS QUICK
mkdir -p ${BUILD_DIR_QGSQUICK}
pushd ${BUILD_DIR_QGSQUICK}


if [ "X${ARCH}" == "Xarmeabi-v7a" ]; then
  export QT_ARCH_PREFIX=armv7
elif [ "X${ARCH}" == "Xarm64-v8a" ]; then
  export QT_ARCH_PREFIX=arm64 # watch out when changing this, openssl depends on it
else
  echo "Error: Please report issue to enable support for arch (${ARCH})."
  exit 1
fi
export ANDROID_CMAKE_LINKER_FLAGS=""
ANDROID_CMAKE_LINKER_FLAGS="$ANDROID_CMAKE_LINKER_FLAGS;-Wl,-rpath=$ANDROID_NDK_ROOT/platforms/android-$ANDROIDAPI/arch-$QT_ARCH_PREFIX/usr/lib"
ANDROID_CMAKE_LINKER_FLAGS="$ANDROID_CMAKE_LINKER_FLAGS;-Wl,-rpath=$ANDROID_NDK_ROOT/sources/cxx-stl/llvm-libc++/libs/$ARCH"
ANDROID_CMAKE_LINKER_FLAGS="$ANDROID_CMAKE_LINKER_FLAGS;-Wl,-rpath=$STAGE_PATH/lib"
ANDROID_CMAKE_LINKER_FLAGS="$ANDROID_CMAKE_LINKER_FLAGS;-Wl,-llog"
ANDROID_CMAKE_LINKER_FLAGS="$ANDROID_CMAKE_LINKER_FLAGS;-Wl,-rpath=$QT_ANDROID/lib"
export STAGE_PATH=/home/input-sdk/${ARCH}

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
    -DQGIS_VERSION_MAJOR=3 \
    -DQGIS_VERSION_MINOR=17 \
    -DQGIS_VERSION_PATCH=0 \
    -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR_QGSQUICK} \
    -DCMAKE_PREFIX_PATH=$QT_ANDROID \
    -DENABLE_TESTS=FALSE \
    -DFORCE_STATIC_LIBS=FALSE \
    -DUSE_QGIS_BUILD_DIR=FALSE \
    -DQGIS_INSTALL_PATH=$STAGE_PATH \
    -DQGIS_CMAKE_PATH=$STAGE_PATH/cmake \
    -DGDAL_CONFIG=$STAGE_PATH/bin/gdal-config \
    -DGDAL_CONFIG_PREFER_FWTOOLS_PAT=/bin_safe \
    -DGDAL_CONFIG_PREFER_PATH=$STAGE_PATH/bin \
    -DGDAL_INCLUDE_DIR=$STAGE_PATH/include \
    -DGDAL_LIBRARY=$STAGE_PATH/lib/libgdal.so \
    -DGEOS_CONFIG=$STAGE_PATH/bin/geos-config \
    -DGEOS_CONFIG_PREFER_PATH=$STAGE_PATH/bin \
    -DGEOS_INCLUDE_DIR=$STAGE_PATH/include \
    -DGEOS_LIBRARY=$STAGE_PATH/lib/libgeos_c.so \
    -DGEOS_LIB_NAME_WITH_PREFIX=-lgeos_c \
    -DICONV_INCLUDE_DIR=$STAGE_PATH/include \
    -DICONV_LIBRARY=$STAGE_PATH/lib/libiconv.so \
    -DSQLITE3_INCLUDE_DIR=$STAGE_PATH/include \
    -DSQLITE3_LIBRARY=$STAGE_PATH/lib/libsqlite3.so \
    -DPOSTGRES_CONFIG= \
    -DPOSTGRES_CONFIG_PREFER_PATH= \
    -DPOSTGRES_INCLUDE_DIR=$STAGE_PATH/include \
    -DPOSTGRES_LIBRARY=$STAGE_PATH/lib/libpq.so \
    -DWITH_BINDINGS=OFF \
    -DWITH_GRASS=OFF \
    -DWITH_GEOREFERENCER=OFF \
    -DWITH_QTMOBILITY=OFF \
    -DQCA_INCLUDE_DIR=$STAGE_PATH/include/Qca-qt5/QtCrypto \
    -DQCA_LIBRARY=$STAGE_PATH/lib/libqca-qt5_$ARCH.so \
    -DQTKEYCHAIN_INCLUDE_DIR=$STAGE_PATH/include/qt5keychain \
    -DQTKEYCHAIN_LIBRARY=$STAGE_PATH/lib/libqt5keychain_$ARCH.so \
    -DCMAKE_INSTALL_PREFIX:PATH=$STAGE_PATH \
    -DENABLE_QT5=ON \
    -DENABLE_TESTS=OFF \
    -DEXPAT_INCLUDE_DIR=$STAGE_PATH/include \
    -DEXPAT_LIBRARY=$STAGE_PATH/lib/libexpat.so \
    -DWITH_INTERNAL_QWTPOLAR=OFF \
    -DWITH_QWTPOLAR=OFF \
    -DWITH_GUI=OFF \
    -DSPATIALINDEX_LIBRARY=$STAGE_PATH/lib/libspatialindex.so \
    -DWITH_APIDOC=OFF \
    -DWITH_ASTYLE=OFF \
    -DWITH_QUICK=OFFv \
    -DWITH_QT5SERIALPORT=OFF \
    -DNATIVE_CRSSYNC_BIN=/usr/bin/true \
    -DWITH_QGIS_PROCESS=OFF \
    -DProtobuf_PROTOC_EXECUTABLE:FILEPATH=$NATIVE_STAGE_PATH/bin/protoc \
    -DProtobuf_INCLUDE_DIRS:PATH=$STAGE_PATH/include \
    -DProtobuf_LIBRARY=$STAGE_PATH/lib/libprotobuf.so \
    -DProtobuf_LITE_LIBRARY=$STAGE_PATH/lib/libprotobuf-lite.so \
    -DProtobuf_PROTOC_LIBRARY=$STAGE_PATH/lib/libprotoc.so \
    -DANDROID_TARGET_ARCH=$ARCH \
  ${SOURCE_DIR}/qgsquick

make -j ${CORES}
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
