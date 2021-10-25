#!/usr/bin/env bash

# -e will make script fail if any command fails
# -u will error script when uninitialized variable is accessed
# -o pipefail will stop on the first error when pipi
set -euo pipefail

if [[ $# -eq 0 || $1 == '-h' || $1 == '--help' ]] ; then
  echo 'android-build.sh Builds android version of Input.
To build release version, use: --release
To build debug version, use:   --debug'

  exit 0
fi

VERSIONTAG=1.1.0
SDK_VERSION=android-22
ARCH=arm64-v8a

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd ${DIR}/..

docker pull lutraconsulting/input-sdk:${SDK_VERSION}

# decrypt keystore
if [[ $1 == '--debug' ]] ; then
    echo 'Building debug version'

   docker run -v $(pwd):/usr/src/input \
       -e "BUILD_FOLDER=build-${ARCH}" \
       -e "ARCH=${ARCH}" \
       -e "VERSION=${VERSIONTAG}" \
       lutraconsulting/input-sdk:${SDK_VERSION} /usr/src/input/scripts/docker-build.sh
else
    echo 'Building release version'

    echo 'KEYSTORE DECRYPT KEY:'
    read -s DECRYPT_KEY

    echo 'KEYSTORE STOREPASS:'
    read -s STOREPASS

    openssl aes-256-cbc -k ${DECRYPT_KEY} -in Input_keystore.keystore.enc -out Input_keystore.keystore -d -md md5

    docker run -v $(pwd):/usr/src/input \
        -e "BUILD_FOLDER=build-${ARCH}" \
        -e "ARCH=${ARCH}" \
        -e "VERSION=${VERSIONTAG}" \
        -e "INPUTKEYSTORE_STOREPASS=${STOREPASS}" \
        lutraconsulting/input-sdk:${SDK_VERSION} /usr/src/input/scripts/docker-build.sh
fi

echo 'Done. APK is generated inside build-<architecture>/out/build/outputs/apk'
