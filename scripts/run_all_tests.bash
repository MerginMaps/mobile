#!/usr/bin/env bash

INPUT_EXECUTABLE=$1

if [ ! -f "$INPUT_EXECUTABLE" ]; then
  echo "Missing Input executable as first argument"
  exit 1;
fi

NFAILURES=0

$INPUT_EXECUTABLE --testLinks
NFAILURES=$(($NFAILURES+$?))

$INPUT_EXECUTABLE --testUtils
NFAILURES=$(($NFAILURES+$?))

$INPUT_EXECUTABLE --testAttributePreviewController
NFAILURES=$(($NFAILURES+$?))

$INPUT_EXECUTABLE --testMerginApi -maxwarnings 0
NFAILURES=$(($NFAILURES+$?))

$INPUT_EXECUTABLE --testPurchasing
NFAILURES=$(($NFAILURES+$?))

$INPUT_EXECUTABLE --testAttributeController
NFAILURES=$(($NFAILURES+$?))

$INPUT_EXECUTABLE --testIdentifyKit
NFAILURES=$(($NFAILURES+$?))

$INPUT_EXECUTABLE --testPosition
NFAILURES=$(($NFAILURES+$?))

$INPUT_EXECUTABLE --testRememberAttributesController
NFAILURES=$(($NFAILURES+$?))

$INPUT_EXECUTABLE --testScaleBarKit
NFAILURES=$(($NFAILURES+$?))

$INPUT_EXECUTABLE --testPurchasing
NFAILURES=$(($NFAILURES+$?))

$INPUT_EXECUTABLE --testFormEditors
NFAILURES=$(($NFAILURES+$?))

$INPUT_EXECUTABLE --testModels
NFAILURES=$(($NFAILURES+$?))

$INPUT_EXECUTABLE --testCoreUtils
NFAILURES=$(($NFAILURES+$?))

echo "Total $NFAILURES failures found in testing"

exit $NFAILURES
