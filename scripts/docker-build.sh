#!/bin/bash

# This is intended to be run within a opengisch/qfield-sdk docker container.

SOURCE_DIR=/usr/src/input
if [[ -z ${BUILD_FOLDER+x} ]]; then
    BUILD_DIR=${SOURCE_DIR}/build-docker
else
    BUILD_DIR=${SOURCE_DIR}/${BUILD_FOLDER}
fi
if [[ -z ${ARCH+x} ]]; then
    ARCH=armv7
fi
INSTALL_DIR=${BUILD_DIR}/out
QT_ANDROID=${QT_ANDROID_BASE}/android_${ARCH}

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
echo "BUILD_DIR: ${BUILD_DIR}"
echo "ARCH: ${ARCH}"

mkdir -p ${BUILD_DIR}/.gradle
# androiddeployqt needs gradle and downloads it to /root/.gradle. By linking it to the build folder, this will be cached between builds.
ln -s ${BUILD_DIR}/.gradle /root/.gradle

pushd ${BUILD_DIR}
cp ${SOURCE_DIR}/scripts/ci/config.pri ${SOURCE_DIR}/app/config.pri
${QT_ANDROID}/bin/qmake ${SOURCE_DIR}/app/input.pro
make
make install INSTALL_ROOT=${INSTALL_DIR}

if [ -n "${STOREPASS}" ]; then
    ${QT_ANDROID}/bin/androiddeployqt \
	    --sign ${SOURCE_DIR}/Input_keystore.keystore "input" \
	    --storepass "${STOREPASS}" \
	    --keypass "${STOREPASS}" \
        --input ${BUILD_DIR}/android-libInput.so-deployment-settings.json \
	    --output ${INSTALL_DIR} \
	    --deployment bundled \
	    --android-platform ${ANDROID_NDK_PLATFORM} \
	    --gradle
else
    ${QT_ANDROID}/bin/androiddeployqt \
        --input ${BUILD_DIR}/android-libInput.so-deployment-settings.json \
	    --output ${INSTALL_DIR} \
	    --deployment bundled \
	    --android-platform ${ANDROID_NDK_PLATFORM} \
	    --gradle
fi

chown -R $(stat -c "%u" .):$(stat -c "%u" .) .
popd
