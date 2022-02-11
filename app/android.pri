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

    # If we do not build 64b ABI, we do not need 64b SDK
    isEmpty(INPUT_ONLY_TARGET_ARCH) {
      exists($${INPUT_SDK_LIB_PATH}/libqgis_core.a) {
        message("Building from QGIS: $${INPUT_SDK_LIB_PATH}/libqgis_core.a")
      } else {
        error("Missing QGIS Core library in $${INPUT_SDK_LIB_PATH}/libqgis_core.a")
      }
    } else {
      equals(INPUT_ONLY_TARGET_ARCH, $${QT_ARCH}) {
        exists($${INPUT_SDK_LIB_PATH}/libqgis_core.a) {
          message("Building from QGIS: $${INPUT_SDK_LIB_PATH}/libqgis_core.a")
        } else {
          error("Missing QGIS Core library in $${INPUT_SDK_LIB_PATH}/libqgis_core.a")
        }
      } else {
        message("Skipping check QGIS Core library for $${QT_ARCH}, INPUT_ONLY_TARGET_ARCH set to $${INPUT_ONLY_TARGET_ARCH}")
      }
    }

    INCLUDEPATH += $${INPUT_SDK_INCLUDE_PATH}
    INCLUDEPATH += $${INPUT_SDK_INCLUDE_PATH}/qgis
    
    # by default QMake eats linked libs if they are mentioned multiple times
    # https://stackoverflow.com/questions/18327959/qmake-how-to-link-a-library-twice/18328971
    CONFIG += no_lflags_merge
    LIBS += -L$${INPUT_SDK_LIB_PATH}

    # a) STATIC libraries before -lunwind
    # see note below about unwinding
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
    LIBS += -lsqlite3 -liconv -lz -lzip
    LIBS += -lwebp
    
    QT += multimedia
    QT += printsupport
    QT += androidextras
    QT += bluetooth
    DEFINES += "HAVE_BLUETOOTH"

    QMAKE_CXXFLAGS += -std=c++11

    # B) see https://android.googlesource.com/platform/ndk/+/master/docs/BuildSystemMaintainers.md#Unwinding
    # note: may not be needed with NDK r23+ (we use r21 ATM)
    # without this it crashes in PROJ's c_locale_stod on start
    # https://github.com/lutraconsulting/input/issues/1824
    equals ( QT_ARCH, 'armeabi-v7a' ) {
        LIBS += -lunwind
    }

    # C) SHARED libraries after -lunwind
    # see note above about unwinding
    LIBS += -lssl_1_1 -lcrypto_1_1

    # files from this folder will be added to the package
    # (and will override any default files from Qt - template is in $QTDIR/src/android)
    system($$PWD/../scripts/patch_manifest.bash $${ANDROID_VERSION_NAME} $${ANDROID_VERSION_CODE} $$PWD/android $$OUT_PWD/android_patched)
    ANDROID_PACKAGE_SOURCE_DIR = $$OUT_PWD/android_patched
    QT_LIBS_DIR = $$dirname(QMAKE_QMAKE)/../lib

    # this makes the manifest visible from Qt Creator
    DISTFILES += $$OUT_PWD/android_patched/AndroidManifest.xml
    DISTFILES += $$OUT_PWD/res/xml/file_paths.xml
    DISTFILES += $$PWD/build.gradle

    for (abi, ANDROID_ABIS): ANDROID_EXTRA_LIBS += \
        $${INPUT_SDK_PATH}/$${abi}/lib/libcrypto_1_1.so \
        $${INPUT_SDK_PATH}/$${abi}/lib/libssl_1_1.so \
        $$QT_LIBS_DIR/libQt5OpenGL_$${abi}.so \
        $$QT_LIBS_DIR/libQt5PrintSupport_$${abi}.so \
        $$QT_LIBS_DIR/libQt5Sensors_$${abi}.so \
        $$QT_LIBS_DIR/libQt5Network_$${abi}.so \
        $$QT_LIBS_DIR/libQt5Sql_$${abi}.so \
        $$QT_LIBS_DIR/libQt5Svg_$${abi}.so \
        $$QT_LIBS_DIR/libQt5AndroidExtras_$${abi}.so \
        $$QT_LIBS_DIR/libQt5SerialPort_$${abi}.so
}
