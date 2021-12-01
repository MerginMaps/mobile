android {
    message("Building ANDROID")
    message("ANDROID Platform: $${QT_ARCH}")

    DEFINES += MOBILE_OS

    isEmpty(INPUT_SDK_PATH) {
      error("Missing INPUT_SDK_PATH")
    }

    INPUT_SDK_ARCH_PATH = $${INPUT_SDK_PATH}/$${QT_ARCH}
    INPUT_SDK_LIB_PATH = $${INPUT_SDK_ARCH_PATH}/lib
    INPUT_SDK_INCLUDE_PATH = $${INPUT_SDK_ARCH_PATH}/include

    exists($${INPUT_SDK_LIB_PATH}/libqgis_core.a) {
      message("Building from QGIS: $${INPUT_SDK_LIB_PATH}/libqgis_core.a")
    } else {
      error("Missing QGIS Core library in $${INPUT_SDK_LIB_PATH}/libqgis_core.a")
    }
    
    INCLUDEPATH += $${INPUT_SDK_INCLUDE_PATH}
    INCLUDEPATH += $${INPUT_SDK_INCLUDE_PATH}/qgis
    
    # by default QMake eats linked libs if they are mentioned multiple times
    # https://stackoverflow.com/questions/18327959/qmake-how-to-link-a-library-twice/18328971
    CONFIG += no_lflags_merge

    LIBS += -L$${INPUT_SDK_LIB_PATH}
    LIBS += -lqgis_core
    LIBS += -lgeodiff
    LIBS += -lproj
    LIBS += -lZXing
    LIBS += -lauthmethod_basic_a
    LIBS += -lauthmethod_esritoken_a
    LIBS += -lauthmethod_identcert_a
    LIBS += -lauthmethod_oauth2_a
    LIBS += -lauthmethod_pkcs12_a
    LIBS += -lauthmethod_pkipaths_a
    LIBS += -lprovider_arcgisfeatureserver_a
    LIBS += -lprovider_arcgismapserver_a
    LIBS += -lprovider_delimitedtext_a
    LIBS += -lprovider_spatialite_a
    LIBS += -lprovider_virtuallayer_a
    LIBS += -lprovider_wcs_a
    LIBS += -lprovider_wfs_a
    LIBS += -lprovider_wms_a
    LIBS += -lprovider_postgres_a

    # needs to be added again because of the cycling dependencies between qgis_core
    # and providers and auth methods
    LIBS += -lqgis_core

    LIBS += -lqt5keychain -lqca-qt5
    LIBS += -lgdal -lpq -lspatialite
    LIBS += -lcharset
    LIBS += -ltasn1 -lproj
    LIBS += -lspatialindex -lgeos_c -lgeos
    LIBS += -lprotobuf-lite -lexpat -lfreexl -lexiv2 -lexiv2-xmp
    LIBS += -lsqlite3 -liconv -lz -lzip -lpng16
    LIBS += -lssl_1_1 -lcrypto_1_1 -lwebp
    
    QT += multimedia
    QT += printsupport
    QT += androidextras

    LOCAL_LDLIBS += -Wl,--no-warn-shared-textrel

    QMAKE_CXXFLAGS += -std=c++11



    # files from this folder will be added to the package
    # (and will override any default files from Qt - template is in $QTDIR/src/android)
    system($$PWD/../scripts/patch_manifest.bash $${ANDROID_VERSION_NAME} $${ANDROID_VERSION_CODE} $$PWD/android $$OUT_PWD/android_patched)
    ANDROID_PACKAGE_SOURCE_DIR = $$OUT_PWD/android_patched
    QT_LIBS_DIR = $$dirname(QMAKE_QMAKE)/../lib

    # this makes the manifest visible from Qt Creator
    DISTFILES += $$OUT_PWD/android_patched/AndroidManifest.xml
    DISTFILES += $$OUT_PWD/res/xml/file_paths.xml
    DISTFILES += $$PWD/build.gradle

    # packaging
    ANDROID_EXTRA_LIBS += \
        $${INPUT_SDK_LIB_PATH}/libcrypto_1_1.so \
        $${INPUT_SDK_LIB_PATH}/libssl_1_1.so \
        $$QT_LIBS_DIR/libQt5OpenGL_$${QT_ARCH}.so \
        $$QT_LIBS_DIR/libQt5PrintSupport_$${QT_ARCH}.so \
        $$QT_LIBS_DIR/libQt5Sensors_$${QT_ARCH}.so \
        $$QT_LIBS_DIR/libQt5Network_$${QT_ARCH}.so \
        $$QT_LIBS_DIR/libQt5Sql_$${QT_ARCH}.so \
        $$QT_LIBS_DIR/libQt5Svg_$${QT_ARCH}.so \
        $$QT_LIBS_DIR/libQt5AndroidExtras_$${QT_ARCH}.so \
        $$QT_LIBS_DIR/libQt5SerialPort_$${QT_ARCH}.so
}
