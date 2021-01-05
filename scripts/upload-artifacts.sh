#!/bin/bash
set -e

ls build-${ARCH}/out/build/outputs/apk/release/
export DROPBOX_FOLDER="android"

export IS_PULL_REQUEST="false" #TODO
if [ -n "${INPUTKEYSTORE_STOREPASS}" ]; then
    export BUILD_FILE=build-${ARCH}/out/build/outputs/apk/release/out-release-signed.apk
    export SIGNED="signed"
    export SECURE_ENV_VARS="true"
else
    export BUILD_FILE=build-${ARCH}/out/build/outputs/apk/debug/android-build-debug.apk
    export SIGNED="debug"
    export SECURE_ENV_VARS="false"
fi
export GITHUB_REPO=lutraconsulting/input

# If we have secure env vars and are in either a pull request or a tag, we need to upload artifacts
if [[ "${SECURE_ENV_VARS}" = "true" ]];
then
  if [ ${IS_PULL_REQUEST} != "false" ]; then
    echo "\e[31mSkip deployment of pull request\e[0m"
    # echo -e "\e[31mDeploying pull request\e[0m"
    # export DROPBOX_FOLDER="pulls"
    # export APK_FILE=input-${IS_PULL_REQUEST}-${GITHUB_SHA}-${ARCH}-${SIGNED}.apk
    # export GITHUB_API=https://api.github.com/repos/${GITHUB_REPO}/issues/${IS_PULL_REQUEST}/comments
    exit 0
  elif [[ -n ${GIT_TAG} ]]; then
    echo -e "\e[31mDeploying tagged release\e[0m"
    export APK_FILE=input-${GIT_TAG}-${GITHUB_SHA}-${ARCH}-${SIGNED}.apk
    export GITHUB_API=https://api.github.com/repos/${GITHUB_REPO}/commits/${GITHUB_SHA}/comments
  elif [[ ${GIT_BRANCH} = master ]]; then
    echo -e "\e[31mDeploying master branch\e[0m"
    # export DROPBOX_FOLDER="master"
    export APK_FILE=input-${GIT_BRANCH}-${GITHUB_SHA}-${ARCH}-${SIGNED}.apk
    export GITHUB_API=https://api.github.com/repos/${GITHUB_REPO}/commits/${GITHUB_SHA}/comments
  else
    echo -e "\e[31mDeploying other commit\e[0m"
    # export DROPBOX_FOLDER="other"
    export APK_FILE=input-${GIT_BRANCH/\//_}-${GITHUB_SHA}-${ARCH}-${SIGNED}.apk
    export GITHUB_API=https://api.github.com/repos/${GITHUB_REPO}/commits/${GITHUB_SHA}/comments
  fi

  echo "copy ${BUILD_FILE} -> $APK_FILE"
  sudo cp ${BUILD_FILE} ${APK_FILE}
  echo "run uploader"
  python3 ./scripts/uploader.py --source ${APK_FILE} --destination "/$DROPBOX_FOLDER/${APK_FILE}" --token DROPBOX_TOKEN --dry
  python3 ./scripts/uploader.py --source ${APK_FILE} --destination "/$DROPBOX_FOLDER/${APK_FILE}" --token DROPBOX_TOKEN > uploader.log 2>&1
  echo "extract APK url"
  APK_URL=`tail -n 1 uploader.log`
  echo "upload APK"
  curl -u inputapp-bot:${GITHUB_TOKEN} -X POST --data '{"body": "'${SIGNED}' apk: ['${ARCH}']('${APK_URL}') (SDK: ['${SDK_VERSION}'](https://github.com/lutraconsulting/input-sdk/releases/tag/'${SDK_VERSION}'))"}' ${GITHUB_API}

else
  echo -e "Not uploading artifacts ..."
  if [ "${SECURE_ENV_VARS}" != "true" ];
  then
    echo -e "  SECURE_ENV_VARS is not true (${SECURE_ENV_VARS})"
  fi
  if [ "${SECURE_ENV_VARS}" = "false" ];
  then
    echo -e "  IS_PULL_REQUEST is false (${IS_PULL_REQUEST})"
  fi
  if [ "${GIT_TAG}X" = "X" ];
  then
    echo -e "  GIT_TAG is not set"
  fi
fi