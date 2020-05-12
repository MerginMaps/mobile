#!/bin/bash
set -e

export ARCH="arm64"
export SIGNED="signed"
export GITHUB_REPO=lutraconsulting/input
export DROPBOX_FOLDER=ios
export BUILD_FILE=$1

if [ -n "${IOS_CERT_KEY}" ]; then
    echo "uploading artifacts to dropbox"
else
    echo "cloned from other fork, nothing to do"
    exit 0
fi

if [ "X${BUILD_FILE}" == "X" ]; then
    echo "missing BUILD_FILE name"
    exit 1
fi

export IPA_FILE=input-ios-${GITHUB_SHA}-${ARCH}-${SIGNED}.apk
export GITHUB_API=https://api.github.com/repos/${GITHUB_REPO}/commits/${GITHUB_SHA}/comments

sudo cp ${BUILD_FILE} /tmp/${IPA_FILE}
python3 ./scripts/uploader.py --source /tmp/${IPA_FILE} --destination "/$DROPBOX_FOLDER/${IPA_FILE}" --token INPUTAPP_BOT_DROPBOX_TOKEN > uploader.log 2>&1
APK_URL=`tail -n 1 uploader.log`
curl -u inputapp-bot:${INPUTAPP_BOT_GITHUB_TOKEN} -X POST --data '{"body": "'${SIGNED}' ipa: ['${ARCH}']('${APK_URL}') (SDK: ['${SDK_VERSION}'](https://github.com/lutraconsulting/input-sdk/releases/tag/ios-'${SDK_VERSION}'))"}' ${GITHUB_API}
