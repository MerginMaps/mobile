macx:!android {
    message("Building MacOS")

    DEFINES += DESKTOP_OS

    # QGIS
    !isEmpty(QGIS_INSTALL_PATH) {
      # using installed QGIS
      QGIS_PREFIX_PATH = $${QGIS_INSTALL_PATH}/QGIS.app/Contents/MacOS
      QGIS_FRAMEWORK_DIR = $${QGIS_INSTALL_PATH}/QGIS.app/Contents/Frameworks
    }

    isEmpty(QGIS_INSTALL_PATH) {
      # using QGIS from build directory (has different layout of directories)
      # expecting QGIS_SRC_DIR and QGIS_BUILD_DIR defined
      QGIS_PREFIX_PATH = $${QGIS_BUILD_DIR}/output
      QGIS_FRAMEWORK_DIR = $${QGIS_BUILD_DIR}/output/lib

      INCLUDEPATH += \
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
          $${QGIS_BUILD_DIR} \
          $${QGIS_BUILD_DIR}/src/core
    }

    exists($${QGIS_FRAMEWORK_DIR}/qgis_core.framework/qgis_core) {
      message("Building from QGIS: $${QGIS_FRAMEWORK_DIR}/qgis_core.framework/qgis_core")
    } else {
      error("Missing QGIS Core library in $${QGIS_FRAMEWORK_DIR}/qgis_core.framework/qgis_core")
    }

    INCLUDEPATH += \
        $${QGIS_FRAMEWORK_DIR}/qgis_native.framework/Headers \
        $${QGIS_FRAMEWORK_DIR}/qgis_core.framework/Headers

    LIBS += -F$${QGIS_FRAMEWORK_DIR}
    LIBS += -framework qgis_core \
            -framework qgis_native

    # Geodiff
    INCLUDEPATH += $${GEODIFF_INCLUDE_DIR}
    LIBS += -L$${GEODIFF_LIB_DIR}
    LIBS += -lgeodiff

    # Proj
    INCLUDEPATH += $${PROJ_INCLUDE_DIR}
    LIBS += -L$${PROJ_LIB_DIR}
    LIBS += -lproj

    # ZXing
    INCLUDEPATH += $${ZXING_INCLUDE_DIR}
    LIBS += -L$${ZXING_LIB_DIR}
    LIBS += -lZXing

    # PURCHASING stuff (only testing)
    DEFINES += "PURCHASING"
    # Uncomment this like to test the real in-app purchases on MacOS
    # DEFINES += "APPLE_PURCHASING"

    # TESTING stuff (only desktop)
    contains(DEFINES, APPLE_PURCHASING) {
       message("Building with native Apple in-app payments on MacOS, disabling INPUT_TEST")
       LIBS += -framework StoreKit -framework Foundation
    } else {
      DEFINES += "INPUT_TEST"
      QT += testlib
      # path to test data
      DEFINES += "INPUT_TEST_DATA_DIR=$$PWD/../test/test_data"
    }

    QT += printsupport
    QT += widgets
    QT += multimedia
    DEFINES += "HAVE_WIDGETS"

    QMAKE_CXXFLAGS += -std=c++11
}
