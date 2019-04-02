#!/bin/bash
set -e

export UPLOAD_ARTIFACT_ID=$( [[ ${TRAVIS_PULL_REQUEST} =~ false ]] && echo ${TRAVIS_TAG} || echo ${TRAVIS_PULL_REQUEST} )
export APK_FILE=input-${UPLOAD_ARTIFACT_ID}-${TRAVIS_COMMIT}-${ARCH}.apk
export BUILD_FILE=/usr/src/input/build-${ARCH}/out/build/outputs/apk/release/out-debug.apk

# If we have secure env vars and are in either a pull request or a tag, we need to upload artifacts
if [[ "${TRAVIS_SECURE_ENV_VARS}" = "true" ]];
then
  if [ ${TRAVIS_PULL_REQUEST} != false ]; then
    export DROPBOX_FOLDER="pulls"
  elif [[ -n ${TRAVIS_TAG} ]]; then
    export DROPBOX_FOLDER="tags"
  elif [[ ${TRAVIS_BRANCH} = master ]]; then
    export DROPBOX_FOLDER="master"
  fi

  sudo cp ${BUILD_FILE} /tmp/${APK_FILE}
  python3 uploader.py --source /tmp/${APK_FILE} --destination "$DROPBOX_FOLDER/${APK_FILE}" --token DROPBOX_TOKEN

else
  echo -e "Not uploading artifacts ..."
  if [ "${TRAVIS_SECURE_ENV_VARS}" != "true" ];
  then
    echo -e "  TRAVIS_SECURE_ENV_VARS is not true (${TRAVIS_SECURE_ENV_VARS})"
  fi
  if [ "${TRAVIS_SECURE_ENV_VARS}" = "false" ];
  then
    echo -e "  TRAVIS_PULL_REQUEST is false (${TRAVIS_PULL_REQUEST})"
  fi
  if [ "${TRAVIS_TAG}X" = "X" ];
  then
    echo -e "  TRAVIS_TAG is not set"
  fi
fi
