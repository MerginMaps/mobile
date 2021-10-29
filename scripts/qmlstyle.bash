#!/bin/bash

# check for Qt 6 install

# iterate over qml dirs and run qmlformat

echo "QML STYLE CHECK STARTED"

echo "Qt 6 install path: " $Qt6_Dir

# bin dir should be added to path already
FORMATTER = $(which qmlformat)

if [ $? -ne 0 ]; then
	echo "[!] qmlformat not found." >&2
	exit 1
fi

echo "Would start qml formatting check! Formatter: "  $FORMATTER

exit 0
