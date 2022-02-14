ios {
    message("Building IOS")

    DEFINES += MOBILE_OS

    CONFIG(debug, debug|release){
        error("Use release with debug info for debugging! Debug not supported due to static linking for the moment.")
    }

    isEmpty(INPUT_SDK_PATH) {
      error("Missing INPUT_SDK_PATH")
    }

    INPUT_SDK_LIB_PATH = $${INPUT_SDK_PATH}/lib
    INPUT_SDK_INCLUDE_PATH = $${INPUT_SDK_PATH}/include

    # QGIS
    QGIS_PREFIX_PATH = $${INPUT_SDK_PATH}/QGIS.app/Contents/MacOS
    QGIS_FRAMEWORK_DIR = $${INPUT_SDK_PATH}/QGIS.app/Contents/Frameworks
    QGIS_PLUGINS_PATH=$${INPUT_SDK_PATH}/QGIS.app/Contents/PlugIns/qgis # is this needed?

    exists($${QGIS_FRAMEWORK_DIR}/qgis_core.framework/qgis_core) {
      message("Building from QGIS: $${QGIS_FRAMEWORK_DIR}/qgis_core.framework/qgis_core")
    } else {
      error("Missing QGIS Core library in $${QGIS_FRAMEWORK_DIR}/qgis_core.framework/qgis_core")
    }

    INCLUDEPATH += \
        $${QGIS_FRAMEWORK_DIR}/qgis_core.framework/Headers \
        $${INPUT_SDK_INCLUDE_PATH}

    LIBS += -F$${QGIS_FRAMEWORK_DIR}
    LIBS += -framework qgis_core

    # Disabling warnings in qgis qgswkbptr.h
    QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-shorten-64-to-32

    QMAKE_TARGET_BUNDLE_PREFIX = LutraConsultingLtd

    CONFIG -= bitcode
    CONFIG += static
    DEFINES += QT_NO_SSL

    QT += multimedia multimediawidgets location
    QTPLUGIN += qios

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
    
    QMAKE_INFO_PLIST = ios/Info.plist

    # demo projects
    demoFiles.files = $$files(android/assets/demo-projects/*)
    demoFiles.path = demo-projects
    QMAKE_BUNDLE_DATA += demoFiles

    # qgis resources
    crsFiles.files = $$files(android/assets/qgis-data/resources/*)
    crsFiles.path = qgis-data/resources
    QMAKE_BUNDLE_DATA += crsFiles

    # proj
    projFiles.files = $$files(android/assets/qgis-data/proj/*)
    projFiles.path = qgis-data/proj
    QMAKE_BUNDLE_DATA += projFiles

    # app icon
    QMAKE_ASSET_CATALOGS = $$PWD/ios/Images.xcassets
    QMAKE_ASSET_CATALOGS_APP_ICON = "AppIcon"

    # launch screen
    app_launch_images.files = $$PWD/ios/launchscreen/InputScreen.xib $$files($$PWD/ios/launchscreen/*.png)
    QMAKE_BUNDLE_DATA += app_launch_images

    HEADERS += \
        ios/iosinterface.h \
        ios/iosviewdelegate.h

    OBJECTIVE_SOURCES += \
        ios/iosinterface.mm \
        ios/iosviewdelegate.mm \
        ios/iosimagepicker.mm \
        ios/iosutils.mm

    DEFINES += "PURCHASING"
    DEFINES += "APPLE_PURCHASING"
    LIBS += -framework StoreKit -framework Foundation

    QMAKE_CXXFLAGS += -std=c++11
}
