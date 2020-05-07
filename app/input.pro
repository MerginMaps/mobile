TEMPLATE = app
TARGET = Input

include(config.pri)
include(version.pri)

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

DEFINES += "QGIS_QUICK_DATA_PATH=$${QGIS_QUICK_DATA_PATH}"
CONFIG(debug, debug|release) {
  DEFINES += "QGIS_PREFIX_PATH=$${QGIS_PREFIX_PATH}"
  DEFINES += "QGIS_QUICK_EXPAND_TEST_DATA"
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $${QGIS_QML_DIR}
