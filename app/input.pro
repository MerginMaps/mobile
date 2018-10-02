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
  QGIS_PREFIX_PATH = $${QGIS_INSTALL_PATH}
  QGIS_LIB_DIR = $${QGIS_INSTALL_PATH}/lib
  QGIS_INCLUDE_DIR = $${QGIS_INSTALL_PATH}/include
  QGIS_QML_DIR = $${QGIS_INSTALL_PATH}/qml
  QT_LIBS_DIR = $$dirname(QMAKE_QMAKE)/../lib

  exists($${QGIS_LIB_DIR}/libqgis_core.so) {
    message("Building from QGIS: $${QGIS_INSTALL_PATH}")
  } else {
    error("Missing QGIS Core library in $${QGIS_LIB_DIR}/libqgis_core.so")
  }


  INCLUDEPATH += \
    $${QGIS_INCLUDE_DIR}/qgis \
    $${QGIS_INCLUDE_DIR}

  LIBS += -L$${QGIS_LIB_DIR}
  LIBS += -lqgis_core -lqgis_quick
}

################
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


SOURCES += \
main.cpp \
projectsmodel.cpp \
layersmodel.cpp \
loader.cpp \
digitizingcontroller.cpp

HEADERS += \
projectsmodel.h \
layersmodel.h \
loader.h \
digitizingcontroller.h

RESOURCES += \
    img/pics.qrc \
    qml/qml.qrc

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
DISTFILES += android/AndroidManifest.xml

# packaging
ANDROID_EXTRA_LIBS += \
    $${QGIS_LIB_DIR}/libcrystax.so \
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
    $${QGIS_LIB_DIR}/libwcsprovider.so \
    $${QGIS_LIB_DIR}/libwfsprovider.so \
    $${QGIS_LIB_DIR}/libwmsprovider.so \
    $$QT_LIBS_DIR/libQt5OpenGL.so \
    $$QT_LIBS_DIR/libQt5PrintSupport.so \
    $$QT_LIBS_DIR/libQt5Sensors.so \
    $$QT_LIBS_DIR/libQt5Sql.so \
    $$QT_LIBS_DIR/libQt5Script.so \
    $$QT_LIBS_DIR/libQt5Svg.so \
    $$QT_LIBS_DIR/libQt5AndroidExtras.so

ANDROID_EXTRA_PLUGINS += $${QGIS_QML_DIR}
