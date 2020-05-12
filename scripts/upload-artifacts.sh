#!/bin/bash
set -e

ls build-${ARCH}/out/build/outputs/apk/release/
export DROPBOX_FOLDER="android"

if [ -n "${STOREPASS}" ]; then
    export BUILD_FILE=build-${ARCH}/out/build/outputs/apk/release/out-release-signed.apk
    export SIGNED="signed"
else
    export BUILD_FILE=build-${ARCH}/out/build/outputs/apk/debug/android-build-debug.apk
    export SIGNED="debug"
fi
export GITHUB_REPO=lutraconsulting/input

# If we have secure env vars and are in either a pull request or a tag, we need to upload artifacts
if [[ "${TRAVIS_SECURE_ENV_VARS}" = "true" ]];
then
  if [ ${TRAVIS_PULL_REQUEST} != "false" ]; then
    echo "\e[31mSkip deployment of pull request\e[0m"
    # echo -e "\e[31mDeploying pull request\e[0m"
    # export DROPBOX_FOLDER="pulls"
    # export APK_FILE=input-${TRAVIS_PULL_REQUEST}-${TRAVIS_COMMIT}-${ARCH}-${SIGNED}.apk
    # export GITHUB_API=https://api.github.com/repos/${GITHUB_REPO}/issues/${TRAVIS_PULL_REQUEST}/comments
    exit 0
  elif [[ -n ${TRAVIS_TAG} ]]; then
    echo -e "\e[31mDeploying tagged release\e[0m"
    export APK_FILE=input-${TRAVIS_TAG}-${TRAVIS_COMMIT}-${ARCH}-${SIGNED}.apk
    export GITHUB_API=https://api.github.com/repos/${GITHUB_REPO}/commits/${TRAVIS_COMMIT}/comments
  elif [[ ${TRAVIS_BRANCH} = master ]]; then
    echo -e "\e[31mDeploying master branch\e[0m"
    # export DROPBOX_FOLDER="master"
    export APK_FILE=input-${TRAVIS_BRANCH}-${TRAVIS_COMMIT}-${ARCH}-${SIGNED}.apk
    export GITHUB_API=https://api.github.com/repos/${GITHUB_REPO}/commits/${TRAVIS_COMMIT}/comments
  else
    echo -e "\e[31mDeploying other commit\e[0m"
    # export DROPBOX_FOLDER="other"
    export APK_FILE=input-${TRAVIS_BRANCH/\//_}-${TRAVIS_COMMIT}-${ARCH}-${SIGNED}.apk
    export GITHUB_API=https://api.github.com/repos/${GITHUB_REPO}/commits/${TRAVIS_COMMIT}/comments
  fi

  sudo cp ${BUILD_FILE} /tmp/${APK_FILE}
  python3 ./scripts/uploader.py --source /tmp/${APK_FILE} --destination "/$DROPBOX_FOLDER/${APK_FILE}" --token DROPBOX_TOKEN > uploader.log 2>&1
  APK_URL=`tail -n 1 uploader.log`
  curl -u inputapp-bot:${GITHUB_TOKEN} -X POST --data '{"body": "'${SIGNED}' apk: ['${ARCH}']('${APK_URL}') (SDK: ['${SDK_VERSION}'](https://github.com/lutraconsulting/input-sdk/releases/tag/'${SDK_VERSION}'))"}' ${GITHUB_API}

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