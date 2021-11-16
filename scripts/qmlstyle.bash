#!/bin/bash

# check for Qt 6 install

# iterate over qml dirs and run qmlformat

echo "QML STYLE CHECK STARTED"

echo "Qt 6 install path: " $Qt6_DIR

# bin dir should be added to path already
which qmlformat
env

if [ $? -ne 0 ]; then
	echo "[!] qmlformat not found." >&2 # TODO: fix action not able to find formatter!
	exit 1
fi

echo "Would start qml formatting check! Formatter: "  $FORMATTER

exit 0
