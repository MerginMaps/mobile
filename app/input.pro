TEMPLATE = app
TARGET = Input

include(config.pri)
include(version.pri)

# Mac specific includes/libraries paths
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
          -framework qgis_native \
          -framework qgis_core
}

# Linux+Android specific includes/libraries paths
!mac {
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

QT += qml quick xml concurrent positioning quickcontrols2
QT += network svg printsupport sql
QT += opengl
android {
    QT += androidextras
}


SOURCES += \
main.cpp \
projectsmodel.cpp \
layersmodel.cpp \
loader.cpp \
digitizingcontroller.cpp \
mapthemesmodel.cpp \
appsettings.cpp \
merginapi.cpp \
merginprojectmodel.cpp \
androidutils.cpp

HEADERS += \
projectsmodel.h \
layersmodel.h \
loader.h \
digitizingcontroller.h \
mapthemesmodel.h \
appsettings.h \
merginapi.h \
merginprojectmodel.h \
androidutils.h

RESOURCES += \
    img/pics.qrc \
    qml/qml.qrc \
    fonts/fonts.qrc

TRANSLATIONS +=

lupdate_only {
    SOURCES += qml/*.qml
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $${QGIS_QML_DIR}

QMAKE_CXXFLAGS += -std=c++11

# files from this folder will be added to the package
# (and will override any default files from Qt - template is in $QTDIR/src/android)
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

# this makes the manifest visible from Qt Creator
DISTFILES += android/AndroidManifest.xml \
    qml/MapThemePanel.qml \
    qml/Notification.qml

# packaging
# Note: libcrypto and libssl currently not included due falling build
android {
    ANDROID_EXTRA_LIBS += \
        $${QGIS_LIB_DIR}/libcrystax.so \
        $${QGIS_LIB_DIR}/libcrypto.so \
        $${QGIS_LIB_DIR}/libexpat.so \
        $${QGIS_LIB_DIR}/libgeos.so \
        $${QGIS_LIB_DIR}/libgeos_c.so \
        $${QGIS_LIB_DIR}/libgslcblas.so \
        $${QGIS_LIB_DIR}/libsqlite3.so \
        $${QGIS_LIB_DIR}/libcharset.so \
        $${QGIS_LIB_DIR}/libiconv.so \
        $${QGIS_LIB_DIR}/libfreexl.so \
        $${QGIS_LIB_DIR}/libtiff.so \
        $${QGIS_LIB_DIR}/libgdal.so \
        $${QGIS_LIB_DIR}/libproj.so \
        $${QGIS_LIB_DIR}/libspatialindex.so \
        $${QGIS_LIB_DIR}/libpq.so \
        $${QGIS_LIB_DIR}/libspatialite.so \
        $${QGIS_LIB_DIR}/libqca-qt5.so \
        $${QGIS_LIB_DIR}/libqgis_core.so \
        $${QGIS_LIB_DIR}/libqgis_quick.so \
        $${QGIS_LIB_DIR}/libqgis_native.so \
        $${QGIS_LIB_DIR}/libqt5keychain.so \
        $${QGIS_LIB_DIR}/libzip.so \
        $${QGIS_LIB_DIR}/libspatialiteprovider.so \
        $${QGIS_LIB_DIR}/libdelimitedtextprovider.so \
        $${QGIS_LIB_DIR}/libgdalprovider.so \
        $${QGIS_LIB_DIR}/libgpxprovider.so \
        $${QGIS_LIB_DIR}/libmssqlprovider.so \
        $${QGIS_LIB_DIR}/libogrprovider.so \
        $${QGIS_LIB_DIR}/libowsprovider.so \
        $${QGIS_LIB_DIR}/libpostgresprovider.so \
        $${QGIS_LIB_DIR}/libspatialiteprovider.so \
        $${QGIS_LIB_DIR}/libssl.so \
        $${QGIS_LIB_DIR}/libwcsprovider.so \
        $${QGIS_LIB_DIR}/libwfsprovider.so \
        $${QGIS_LIB_DIR}/libwmsprovider.so \
        $$QT_LIBS_DIR/libQt5OpenGL.so \
        $$QT_LIBS_DIR/libQt5PrintSupport.so \
        $$QT_LIBS_DIR/libQt5Sensors.so \
        $$QT_LIBS_DIR/libQt5Network.so \
        $$QT_LIBS_DIR/libQt5Sql.so \
        $$QT_LIBS_DIR/libQt5Svg.so \
        $$QT_LIBS_DIR/libQt5AndroidExtras.so

    ANDROID_EXTRA_PLUGINS += $${QGIS_QML_DIR}
}

