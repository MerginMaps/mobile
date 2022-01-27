macx:!android {
    message("Building MacOS")

    DEFINES += DESKTOP_OS

    isEmpty(INPUT_SDK_PATH) {
      error("Missing INPUT_SDK_PATH")
    }
    
    INPUT_SDK_LIB_PATH = $${INPUT_SDK_PATH}/lib
    INPUT_SDK_INCLUDE_PATH = $${INPUT_SDK_PATH}/include
    
    # QGIS
    QGIS_PREFIX_PATH = $${INPUT_SDK_PATH}/QGIS.app/Contents/MacOS
    QGIS_FRAMEWORK_DIR = $${INPUT_SDK_PATH}/QGIS.app/Contents/Frameworks
    QGIS_PLUGINS_PATH=$${INPUT_SDK_PATH}/QGIS.app/Contents/PlugIns/qgis
    
    exists($${QGIS_FRAMEWORK_DIR}/qgis_core.framework/qgis_core) {
      message("Building from QGIS: $${QGIS_FRAMEWORK_DIR}/qgis_core.framework/qgis_core")
    } else {
      error("Missing qgis_core Framework in $${QGIS_FRAMEWORK_DIR}/qgis_core.framework/qgis_core")
    }

    INCLUDEPATH += \
        $${QGIS_FRAMEWORK_DIR}/qgis_native.framework/Headers \
        $${QGIS_FRAMEWORK_DIR}/qgis_core.framework/Headers \
        $${INPUT_SDK_INCLUDE_PATH}

    LIBS += -F$${QGIS_FRAMEWORK_DIR}
    LIBS += -framework qgis_core \
            -framework qgis_native

	# other libs
    LIBS += -L$${INPUT_SDK_LIB_PATH} -L$${QGIS_PLUGINS_PATH}
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
    LIBS += -lqt5keychain -lqca-qt5
    LIBS += -lgdal -lpq -lspatialite
    LIBS += -lcharset -lxml2
    LIBS += -ltasn1 -lbz2 -lproj
    LIBS += -lspatialindex -lgeos -lgeos_c
    LIBS += -lprotobuf-lite -lexpat -lfreexl -lexiv2 -lexiv2-xmp
    LIBS += -lsqlite3 -liconv -lz -lzip
    LIBS += -lwebp

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
    QT += bluetooth
    DEFINES += "HAVE_BLUETOOTH"
    DEFINES += "HAVE_WIDGETS"

    QMAKE_CXXFLAGS += -std=c++11 -fvisibility-inlines-hidden -fvisibility=hidden
}
