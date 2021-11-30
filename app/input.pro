TEMPLATE = app
TARGET = Input
CONFIG += c++17

include(config.pri)
include(version.pri)

isEmpty(QGIS_QUICK_DATA_PATH) {
  error("Missing QGIS_QUICK_DATA_PATH")
}

QT += quick qml xml concurrent positioning sensors quickcontrols2
QT += network svg sql
QT += opengl
QT += core

include(android.pri)
include(ios.pri)
include(linux.pri)
include(macx.pri)
include(win32.pri)
include(sources.pri)
include($$PWD/../core/core.pri)
include($$PWD/../qgsquick/qgsquick.pri)

INCLUDEPATH += \
  $$PWD/../core \
  $$PWD/../qgsquick \
  $$PWD/attributes \
  $$PWD/editor

DEFINES += INPUT_APP
DEFINES += "QGIS_QUICK_DATA_PATH=$${QGIS_QUICK_DATA_PATH}"
CONFIG(debug, debug|release) {
  DEFINES += "QGIS_PREFIX_PATH=$${QGIS_PREFIX_PATH}"
  DEFINES += "QGIS_QUICK_EXPAND_TEST_DATA"
}

contains(DEFINES, INPUT_TEST) {
  TEST_DATA_DIR="$$PWD/../test/test_data"
  DEFINES += "TEST_DATA_DIR=\\\"$${TEST_DATA_DIR}\\\""
}
