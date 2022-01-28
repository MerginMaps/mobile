!mac:!ios:!win32:!android {
    message("Building LINUX")

    DEFINES += DESKTOP_OS

    isEmpty(INPUT_SDK_PATH) {
      error("Missing INPUT_SDK_PATH")
    }

    INPUT_SDK_LIB_PATH = $${INPUT_SDK_PATH}/lib
    INPUT_SDK_INCLUDE_PATH = $${INPUT_SDK_PATH}/include

    exists($${INPUT_SDK_LIB_PATH}/libqgis_core.a) {
      message("Building from QGIS: $${INPUT_SDK_LIB_PATH}/libqgis_core.a")
    } else {
	  error("Missing QGIS Core library in $${INPUT_SDK_LIB_PATH}/libqgis_core.a")
    }

    INCLUDEPATH += $${INPUT_SDK_INCLUDE_PATH}
    INCLUDEPATH += $${INPUT_SDK_INCLUDE_PATH}/qgis
    INCLUDEPATH += /usr/include/gdal

    LIBS += -L$${INPUT_SDK_LIB_PATH}
    LIBS += -L$${INPUT_SDK_LIB_PATH}/x86_64-linux-gnu
    LIBS += -L$${INPUT_SDK_LIB_PATH}/qgis/plugins

    # by default QMake eats linked libs if they are mentioned multiple times
    # https://stackoverflow.com/questions/18327959/qmake-how-to-link-a-library-twice/18328971
    CONFIG += no_lflags_merge

    LIBS += -lqgis_native
    LIBS += -lqgis_core
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

    LIBS += -lgeodiff
    LIBS += -lproj
    LIBS += -lZXing
    LIBS += -lqt5keychain -lqca-qt5
    LIBS += -lgdal -lpq -lspatialite
    LIBS += -lxml2
    LIBS += -lproj
    LIBS += -lspatialindex -lgeos -lgeos_c
    LIBS += -lprotobuf-lite -lexpat -lfreexl -lexiv2 -lexiv2-xmp
    LIBS += -lsqlite3 -lz -lzip
    LIBS += -lwebp

    # TESTING stuff (only desktop)
    DEFINES += "INPUT_TEST"
    QT += testlib
    # path to test data
    DEFINES += "INPUT_TEST_DATA_DIR=$$PWD/../test/test_data"

    # PURCHASING stuff (only testing)
    DEFINES += "PURCHASING"

    QT += printsupport
    QT += widgets
    QT += multimedia
    QT += bluetooth
    DEFINES += "HAVE_BLUETOOTH"
    DEFINES += "HAVE_WIDGETS"

    QMAKE_CXXFLAGS += -std=c++11
}
