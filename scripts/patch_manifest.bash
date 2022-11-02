#!/bin/bash

set -e

BASH_REPLACE_VERSION=$1
BASH_REPLACE_CODE=$2
INFOLDER=$3
OUTFOLDER=$4
MANIFEST=$OUTFOLDER/AndroidManifest.xml

case $OSTYPE in
  darwin*)
    SED="sed -i .orig"
    ;;
  *)
    SED="sed -i"
    ;;
esac

if [ ! -d $INFOLDER ]; then
    echo "Dir $INFOLDER not found!"
fi

mkdir $OUTFOLDER
cp -R $INFOLDER/* $OUTFOLDER
$SED "s/__BASH_REPLACE_VERSION__/$BASH_REPLACE_VERSION/g" $MANIFEST
$SED "s/__BASH_REPLACE_CODE__/$BASH_REPLACE_CODE/g" $MANIFEST
rm -f $OUTFOLDER/AndroidManifest.xml.orig
