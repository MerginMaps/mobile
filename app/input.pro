TEMPLATE = app
TARGET = Input

include(config.pri)
include(version.pri)

# some defines to simplify C++ code
ios {
  DEFINES += MOBILE_OS
}
android {
  DEFINES += MOBILE_OS
}
!android:!ios {
  DEFINES += DESKTOP_OS
}

# Mac+iOS specific includes/libraries paths
mac {
  QGIS_QML_DIR = $${QGIS_INSTALL_PATH}/QGIS.app/Contents/MacOS/qml
  QGIS_PREFIX_PATH = $${QGIS_INSTALL_PATH}/QGIS.app/Contents/MacOS

  QGIS_QUICK_FRAMEWORK = $${QGIS_INSTALL_PATH}/QGIS.app/Contents/MacOS/lib/qgis_quick.framework
  QGIS_NATIVE_FRAMEWORK = $${QGIS_INSTALL_PATH}/QGIS.app/Contents/Frameworks/qgis_native.framework
  QGIS_CORE_FRAMEWORK = $${QGIS_INSTALL_PATH}/QGIS.app/Contents/Frameworks/qgis_core.framework

  exists($${QGIS_CORE_FRAMEWORK}/qgis_core) {
    message("Building from QGIS: $${QGIS_INSTALL_PATH}")
  } else {
    error("Missing qgis_core Framework in $${QGIS_CORE_FRAMEWORK}/qgis_core")
  }

  INCLUDEPATH += \
    $${QGIS_QUICK_FRAMEWORK}/Headers \
    $${QGIS_NATIVE_FRAMEWORK}/Headers \
    $${QGIS_CORE_FRAMEWORK}/Headers

  LIBS += -F$${QGIS_INSTALL_PATH}/QGIS.app/Contents/MacOS/lib  \
          -F$${QGIS_INSTALL_PATH}/QGIS.app/Contents/Frameworks

  LIBS += -framework qgis_quick \
          -framework qgis_core
}

# MacOS only
macx {
  LIBS +=  -framework qgis_native
}

# ios specific includes/libraries paths
ios {
    QGIS_LIB_DIR = $${QGIS_INSTALL_PATH}/lib
    QGIS_PROVIDER_DIR = $${QGIS_INSTALL_PATH}/QGIS.app/Contents/PlugIns/qgis

    CONFIG -= bitcode
    CONFIG += static
    DEFINES += QT_NO_SSL

    QTPLUGIN += qsvg qsvgicon qtsensors_ios qios qtposition_cl qsqlite

    LIBS += -L$${QGIS_LIB_DIR} -L$${QGIS_PROVIDER_DIR} -L$${QGIS_QML_DIR}/QgsQuick/ -L$${QGIS_QML_DIR}/../lib/
    LIBS += -lgeos -lqt5keychain -lqca-qt5 -lgdal
    LIBS += -lexpat -lgsl -lgslcblas -lcharset -lfreexl
    LIBS += -ltiff -lgdal -lproj -lspatialindex -lpq -lspatialite -lqca-qt5 -ltasn1
    LIBS += -lzip -liconv -lbz2
    LIBS += -lqgis_quick_plugin

    RESOURCES += $$QGIS_QML_DIR/QgsQuick/qgsquick.qrc
    QMAKE_RPATHDIR += @executable_path/../Frameworks

    QMAKE_INFO_PLIST = ios/Info.plist
}

# Linux+Android specific includes/libraries paths
!mac:!ios {
  QT_LIBS_DIR = $$dirname(QMAKE_QMAKE)/../lib

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
    QGIS_QML_DIR = $${QGIS_BUILD_DIR}/output/qml
    QGIS_INCLUDE_DIR = \
      $${QGIS_SRC_DIR}/src/core \
      $${QGIS_SRC_DIR}/src/core/annotations \
      $${QGIS_SRC_DIR}/src/core/auth \
      $${QGIS_SRC_DIR}/src/core/composer \
      $${QGIS_SRC_DIR}/src/core/fieldformatter \
      $${QGIS_SRC_DIR}/src/core/geometry \
      $${QGIS_SRC_DIR}/src/core/layertree \
      $${QGIS_SRC_DIR}/src/core/layout \
      $${QGIS_SRC_DIR}/src/core/locator \
      $${QGIS_SRC_DIR}/src/core/metadata \
      $${QGIS_SRC_DIR}/src/core/providers/memory \
      $${QGIS_SRC_DIR}/src/core/raster \
      $${QGIS_SRC_DIR}/src/core/scalebar \
      $${QGIS_SRC_DIR}/src/core/symbology \
      $${QGIS_SRC_DIR}/src/core/effects \
      $${QGIS_SRC_DIR}/src/core/metadata \
      $${QGIS_SRC_DIR}/src/core/expression \
      $${QGIS_SRC_DIR}/src/quickgui \
      $${QGIS_SRC_DIR}/src/quickgui/attributes \
      $${QGIS_BUILD_DIR} \
      $${QGIS_BUILD_DIR}/src/core \
      $${QGIS_BUILD_DIR}/src/quickgui
  }

  exists($${QGIS_LIB_DIR}/libqgis_core.so) {
    message("Building from QGIS: $${QGIS_INSTALL_PATH}")
  } else {
    error("Missing QGIS Core library in $${QGIS_LIB_DIR}/libqgis_core.so")
  }

  INCLUDEPATH += $${QGIS_INCLUDE_DIR}
  LIBS += -L$${QGIS_LIB_DIR}
  LIBS += -lqgis_core -lqgis_quick
}

DEFINES += "CORE_EXPORT="
DEFINES += "QUICK_EXPORT="
DEFINES += "QGIS_QUICK_DATA_PATH=$${QGIS_QUICK_DATA_PATH}"

CONFIG(debug, debug|release) {
  DEFINES += "QGIS_PREFIX_PATH=$${QGIS_PREFIX_PATH}"
  DEFINES += "QGIS_QUICK_EXPAND_TEST_DATA"
}

QT += quick qml xml concurrent positioning quickcontrols2
QT += network svg sql
QT += opengl
QT += testlib

ios {
  QT += core
}
!ios {
  QT += printsupport
}
android {
  QT += androidextras
}

# path to test data
DEFINES += "INPUT_TEST_DATA_DIR=$$PWD/../test/test_data"

include(sources.pri)

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $${QGIS_QML_DIR}

QMAKE_CXXFLAGS += -std=c++11

include(android.pri)
include(ios.pri)
