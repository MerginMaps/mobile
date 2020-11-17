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



  export CFLAGS="-DANDROID -fomit-frame-pointer "
  # --sysroot $NDKPLATFORM -I$STAGE_PATH/include"
  # export CFLAGS="$CFLAGS -Wno-unused-command-line-argument"
  export CFLAGS="$CFLAGS -L$ANDROID_NDK_ROOT/sources/cxx-stl/llvm-libc++/libs/$ARCH -isystem $ANDROID_NDK_ROOT/sources/cxx-stl/llvm-libc++/include"
  export CFLAGS="$CFLAGS -isystem $ANDROID_NDK_ROOT/sysroot/usr/include -isystem $ANDROID_NDK_ROOT/sysroot/usr/include/$TOOLCHAIN_SHORT_PREFIX "
  export CFLAGS="$CFLAGS -D__ANDROID_API__=$ANDROIDAPI"

  export CXXFLAGS="$CFLAGS"
  export CPPFLAGS="$CFLAGS"

  # export LDFLAGS="-lm -L$STAGE_PATH/lib"
  export LDFLAGS="$LDFLAGS -L$ANDROID_NDK_ROOT/sources/cxx-stl/llvm-libc++/libs/$ARCH"
  # export LDFLAGS="$LDFLAGS -L$ANDROID_NDK_ROOT/toolchains/llvm/prebuilt/$PYPLATFORM-x86_64/sysroot/usr/lib/$TOOLCHAIN_PREFIX/$ANDROIDAPI"

  if [ "X${ARCH}" == "Xarmeabi-v7a" ]; then
      # make sure that symbols from the following system libs are not exported - on 32-bit ARM this was causing crashes when unwinding
      # stack when handling c++ exceptions. In our case, sqlite3.so exported some unwinding-related symbols which were being picked up
      # by libproj.so and causing havoc.
      # https://android.googlesource.com/platform/ndk/+/master/docs/BuildSystemMaintainers.md#Unwinding
      # https://github.com/android/ndk/issues/785
      # https://github.com/android/ndk/issues/379
      # https://github.com/lutraconsulting/input/issues/641
      export LDFLAGS="$LDFLAGS -Wl,--exclude-libs,libgcc.a -Wl,--exclude-libs,libgcc_real.a -Wl,--exclude-libs,libunwind.a"
  fi

export ANDROID_CMAKE_LINKER_FLAGS=""
ANDROID_CMAKE_LINKER_FLAGS="$ANDROID_CMAKE_LINKER_FLAGS;-Wl,-rpath=$ANDROID_NDK_ROOT/platforms/android-$ANDROIDAPI/arch-$QT_ARCH_PREFIX/usr/lib"
ANDROID_CMAKE_LINKER_FLAGS="$ANDROID_CMAKE_LINKER_FLAGS;-Wl,-rpath=$ANDROID_NDK_ROOT/sources/cxx-stl/llvm-libc++/libs/$ARCH"
export LDFLAGS="$LDFLAGS -Wl,-lc++_shared"
ANDROID_CMAKE_LINKER_FLAGS="$ANDROID_CMAKE_LINKER_FLAGS;-Wl,-rpath=$STAGE_PATH/lib"
ANDROID_CMAKE_LINKER_FLAGS="$ANDROID_CMAKE_LINKER_FLAGS;-Wl,-llog"
ANDROID_CMAKE_LINKER_FLAGS="$ANDROID_CMAKE_LINKER_FLAGS;-Wl,-rpath=$QT_ANDROID/lib"

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
    -DGDAL_INCLUDE_DIR=$STAGE_PATH/include \
    -DGDAL_LIBRARY=$STAGE_PATH/lib/libgdal.so \
    -DGEOS_CONFIG=$STAGE_PATH/bin/geos-config \
    -DGEOS_INCLUDE_DIR=$STAGE_PATH/include \
    -DGEOS_LIBRARY=$STAGE_PATH/lib/libgeos_c.so \
    -DSQLITE3_INCLUDE_DIR=$STAGE_PATH/include \
    -DSQLITE3_LIBRARY=$STAGE_PATH/lib/libsqlite3.so \
    -DQCA_INCLUDE_DIR=$STAGE_PATH/include/Qca-qt5/QtCrypto \
    -DQCA_LIBRARY=$STAGE_PATH/lib/libqca-qt5_$ARCH.so \
    -DQTKEYCHAIN_INCLUDE_DIR=$STAGE_PATH/include/qt5keychain \
    -DQTKEYCHAIN_LIBRARY=$STAGE_PATH/lib/libqt5keychain_$ARCH.so \
    -DENABLE_QT5=ON \
    -DENABLE_TESTS=OFF \
    -DEXPAT_INCLUDE_DIR=$STAGE_PATH/include \
    -DEXPAT_LIBRARY=$STAGE_PATH/lib/libexpat.so \
    -DSPATIALINDEX_LIBRARY=$STAGE_PATH/lib/libspatialindex.so \
    -DNATIVE_CRSSYNC_BIN=/usr/bin/true \
    -DANDROID_TARGET_ARCH=$ARCH \
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
