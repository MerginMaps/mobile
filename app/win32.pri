win32 {
  message("Building WINDOWS")

  DEFINES += MOBILE_OS
  CONFIG += windows

  QT_LIBS_DIR = $$dirname(QMAKE_QMAKE)/../lib

  # QGIS
  !isEmpty(QGIS_INSTALL_PATH) {
      # using installed QGIS
      QGIS_PREFIX_PATH = $${QGIS_INSTALL_PATH}
      QGIS_LIB_DIR = $${QGIS_INSTALL_PATH}/lib
      QGIS_INCLUDE_DIR = $${QGIS_INSTALL_PATH}/include
      QGIS_QML_DIR = $${QGIS_INSTALL_PATH}/qml
  }

  exists($${QGIS_LIB_DIR}/qgis_core.lib) {
    message("Building from QGIS: $${QGIS_INSTALL_PATH}")
  } else {
    error("Missing QGIS Core library in $${QGIS_LIB_DIR}/qgis_core.lib")
  }

  INCLUDEPATH += $${QGIS_INCLUDE_DIR}
  LIBS += -L$${QGIS_LIB_DIR}
  LIBS += -lqgis_core

  # QGSQUICK
  !isEmpty(QGSQUICK_INSTALL_PATH) {
      # using installed QGSQUICK
      QGSQUICK_LIB_DIR = $${QGSQUICK_INSTALL_PATH}/lib
      QGSQUICK_INCLUDE_DIR = $${QGSQUICK_INSTALL_PATH}/include
      QGSQUICK_QML_DIR = $${QGSQUICK_INSTALL_PATH}/qml
  }

  exists($${QGSQUICK_LIB_DIR}/qgis_quick.lib) {
    message("Building from QGSQUICK: $${QGSQUICK_INSTALL_PATH}")
  } else {
    error("Missing QGSQUICK library in $${QGSQUICK_LIB_DIR}/qgis_quick.lib")
  }

  INCLUDEPATH += $${QGSQUICK_INCLUDE_DIR}
  LIBS += -L$${QGSQUICK_LIB_DIR}
  LIBS += -lqgis_quick

  # Geodiff
  INCLUDEPATH += $${GEODIFF_INCLUDE_DIR}
  LIBS += -L$${GEODIFF_LIB_DIR}
  LIBS += -lgeodiff

  QT += printsupport

  CONFIG += c++11
  DEFINES += "_USE_MATH_DEFINES"

  # Disable: warning C4003: not enough actual parameters for macro 'QStringLiteral'
  QMAKE_CXXFLAGS += /wd4003
}
