ios {
    message("Building IOS")

    DEFINES += MOBILE_OS

    CONFIG(debug, debug|release){
        error("Use release with debug info for debugging! Debug not supported due to static linking for the moment.")
    }

    # QGIS
    QGIS_PREFIX_PATH = $${QGIS_INSTALL_PATH}/QGIS.app/Contents/MacOS
    QGIS_CORE_FRAMEWORK = $${QGIS_INSTALL_PATH}/QGIS.app/Contents/Frameworks/qgis_core.framework

    exists($${QGIS_CORE_FRAMEWORK}/qgis_core) {
      message("Building from QGIS: $${QGIS_INSTALL_PATH}")
    } else {
      error("Missing qgis_core Framework in $${QGIS_CORE_FRAMEWORK}/qgis_core")
    }

    INCLUDEPATH += $${QGIS_CORE_FRAMEWORK}/Headers

    LIBS += -F$${QGIS_INSTALL_PATH}/QGIS.app/Contents/Frameworks
    LIBS += -framework qgis_core

    # Geodiff
    INCLUDEPATH += $${GEODIFF_INCLUDE_DIR}
    LIBS += -L$${GEODIFF_LIB_DIR}
    LIBS += -lgeodiff

    # Proj
    INCLUDEPATH += $${PROJ_INCLUDE_DIR}
    LIBS += -L$${PROJ_LIB_DIR}
    LIBS += -lproj

#    # ZXing
    INCLUDEPATH += $${ZXING_INCLUDE_DIR}
    LIBS += -L$${ZXING_LIB_DIR}
    LIBS += -lZXing

    # Disabling warnings in qgis qgswkbptr.h
    QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-shorten-64-to-32

    QMAKE_TARGET_BUNDLE_PREFIX = LutraConsultingLtd

    CONFIG -= bitcode
    CONFIG += static
    DEFINES += QT_NO_SSL

    QT += multimedia multimediawidgets location
    QTPLUGIN += qios

    LIBS += -L$${QGIS_INSTALL_PATH}/lib -L$${QGIS_INSTALL_PATH}/QGIS.app/Contents/PlugIns/qgis
    LIBS += -lwmsprovider_a -lpostgresprovider_a
    LIBS += -lqt5keychain -lqca-qt5
    LIBS += -lgdal -lpq -lspatialite
    LIBS += -lcharset -lxml2
    LIBS += -ltasn1 -lzip -lbz2 -lproj
    LIBS += -lspatialindex -lgeos -lgeos_c -lsqlite3
    LIBS += -lprotobuf-lite -lexpat -lfreexl -liconv

    QMAKE_RPATHDIR += @executable_path/../Frameworks
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
