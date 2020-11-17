!mac:!ios:!win32:!android {
    message("Building LINUX")

    DEFINES += DESKTOP_OS

    # QGIS
    !isEmpty(QGIS_INSTALL_PATH) {
      # using installed QGIS
      QGIS_PREFIX_PATH = $${QGIS_INSTALL_PATH}
      QGIS_LIB_DIR = $${QGIS_INSTALL_PATH}/lib
      QGIS_INCLUDE_DIR = $${QGIS_INSTALL_PATH}/include/qgis
      QGIS_QML_DIR = $${QGIS_INSTALL_PATH}/qml
    }

    isEmpty(QGIS_INSTALL_PATH) {
      # using QGIS from build directory (has different layout of directories)
      # expecting QGIS_SRC_DIR and QGIS_BUILD_DIR defined
      QGIS_PREFIX_PATH = $${QGIS_BUILD_DIR}/output
      QGIS_LIB_DIR = $${QGIS_BUILD_DIR}/output/lib
      QGIS_INCLUDE_DIR = \
          $${QGIS_SRC_DIR}/src/core \
          $${QGIS_SRC_DIR}/src/core/annotations \
          $${QGIS_SRC_DIR}/src/core/auth \
          $${QGIS_SRC_DIR}/src/core/composer \
          $${QGIS_SRC_DIR}/src/core/effects \
          $${QGIS_SRC_DIR}/src/core/expression \
          $${QGIS_SRC_DIR}/src/core/fieldformatter \
          $${QGIS_SRC_DIR}/src/core/geometry \
          $${QGIS_SRC_DIR}/src/core/labeling \
          $${QGIS_SRC_DIR}/src/core/layertree \
          $${QGIS_SRC_DIR}/src/core/layout \
          $${QGIS_SRC_DIR}/src/core/locator \
          $${QGIS_SRC_DIR}/src/core/metadata \
          $${QGIS_SRC_DIR}/src/core/providers/memory \
          $${QGIS_SRC_DIR}/src/core/raster \
          $${QGIS_SRC_DIR}/src/core/scalebar \
          $${QGIS_SRC_DIR}/src/core/symbology \
          $${QGIS_SRC_DIR}/src/core/textrenderer \
          $${QGIS_SRC_DIR}/external/nlohmann \
          $${QGIS_BUILD_DIR} \
          $${QGIS_BUILD_DIR}/src/core
    }

    exists($${QGIS_LIB_DIR}/libqgis_core.so) {
      message("Building from QGIS: $${QGIS_LIB_DIR}/libqgis_core.so")
    } else {
	  error("Missing QGIS Core library in $${QGIS_LIB_DIR}/libqgis_core.so")
    }

    INCLUDEPATH += $${QGIS_INCLUDE_DIR}
    LIBS += -L$${QGIS_LIB_DIR}
    LIBS += -lqgis_core

    # QgsQuick
    !isEmpty(QGSQUICK_INSTALL_PATH) {
      # using installed QGSQUICK
      QGSQUICK_LIB_DIR = $${QGSQUICK_INSTALL_PATH}/lib
      QGSQUICK_INCLUDE_DIR = $${QGSQUICK_INSTALL_PATH}/include
      QGSQUICK_QML_DIR = $${QGSQUICK_INSTALL_PATH}/qml
    }

    isEmpty(QGSQUICK_INSTALL_PATH) {
      # using QGSQUICK from build directory (has different layout of directories)
      # expecting QGIS_SRC_DIR and QGSQUICK_BUILD_DIR defined
      QGSQUICK_LIB_DIR = $${QGSQUICK_BUILD_DIR}/output/lib
      QGSQUICK_QML_DIR = $${QGSQUICK_BUILD_DIR}/output
      QGSQUICK_INCLUDE_DIR = \
        $$PWD/../qgsquick/from_qgis \
        $$PWD/../qgsquick/from_qgis/attributes
    }

    exists($${QGSQUICK_LIB_DIR}/libqgis_quick.so) {
      message("Building from QGSQUICK: $${QGSQUICK_LIB_DIR}/libqgis_quick.so")
    } else {
	  error("Missing QGSQUICK library in $${QGSQUICK_LIB_DIR}/libqgis_quick.so")
    }

    INCLUDEPATH += $${QGSQUICK_INCLUDE_DIR}
    LIBS += -L$${QGSQUICK_LIB_DIR}
    LIBS += -lqgis_quick

    # Geodiff
    INCLUDEPATH += $${GEODIFF_INCLUDE_DIR}
    LIBS += -L$${GEODIFF_LIB_DIR}
    LIBS += -lgeodiff

    # TESTING stuff (only desktop)
    DEFINES += "INPUT_TEST"
    QT += testlib
    # path to test data
    DEFINES += "INPUT_TEST_DATA_DIR=$$PWD/../test/test_data"

    # PURCHASING stuff (only testing)
    DEFINES += "PURCHASING"

    QT += printsupport
    QT += widgets
    DEFINES += "HAVE_WIDGETS"

    QMAKE_CXXFLAGS += -std=c++11
}
