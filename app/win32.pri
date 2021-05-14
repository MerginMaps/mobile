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
  }

  exists($${QGIS_LIB_DIR}/qgis_core.lib) {
    message("Building from QGIS: $${QGIS_INSTALL_PATH}")
  } else {
    error("Missing QGIS Core library in $${QGIS_LIB_DIR}/qgis_core.lib")
  }

  INCLUDEPATH += $${QGIS_INCLUDE_DIR}
  LIBS += -L$${QGIS_LIB_DIR}
  LIBS += -lqgis_core

  # Geodiff
  INCLUDEPATH += $${GEODIFF_INCLUDE_DIR}
  LIBS += -L$${GEODIFF_LIB_DIR}
  LIBS += -lgeodiff

  # Proj
  INCLUDEPATH += $${PROJ_INCLUDE_DIR}
  LIBS += -L$${PROJ_LIB_DIR}
  LIBS += -lproj

  QT += printsupport

  CONFIG += c++11
  DEFINES += "_USE_MATH_DEFINES"

  # Disable: warning C4003: not enough actual parameters for macro 'QStringLiteral'
  QMAKE_CXXFLAGS += /wd4003
}
