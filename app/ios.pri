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

    # QgsQuick
    QGSQUICK_QML_DIR = $${QGSQUICK_INSTALL_PATH}/qml
    QGSQUICK_QUICK_FRAMEWORK = $${QGSQUICK_INSTALL_PATH}/frameworks/qgis_quick.framework

    exists($${QGSQUICK_QUICK_FRAMEWORK}/qgis_quick) {
      message("Building from QGSQUICK: $${QGSQUICK_INSTALL_PATH}")
    } else {
      error("Missing qgis_quick Framework in $${QGSQUICK_QUICK_FRAMEWORK}/qgis_quick")
    }
    INCLUDEPATH += $${QGSQUICK_QUICK_FRAMEWORK}/Headers
    LIBS += -F$${QGSQUICK_INSTALL_PATH}/frameworks
    LIBS += -framework qgis_quick

    # Geodiff
    INCLUDEPATH += $${GEODIFF_INCLUDE_DIR}
    LIBS += -L$${GEODIFF_LIB_DIR}
    LIBS += -lgeodiff

    # Disabling warnings in qgis qgswkbptr.h
    QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-shorten-64-to-32

    QMAKE_TARGET_BUNDLE_PREFIX = LutraConsultingLtd
    QGSQUICK_IMAGE_DIR = $${QGSQUICK_INSTALL_PATH}/images/QgsQuick

    CONFIG -= bitcode
    CONFIG += static
    DEFINES += QT_NO_SSL

    QT += multimedia multimediawidgets location
    QTPLUGIN += qios

    LIBS += -L$${QGIS_INSTALL_PATH}/lib -L$${QGIS_INSTALL_PATH}/QGIS.app/Contents/PlugIns/qgis
    LIBS += -L$${QGSQUICK_QML_DIR}/QgsQuick/ -L$${QGSQUICK_INSTALL_PATH}/frameworks
    LIBS += -lqgis_quick_plugin
    LIBS += -lwmsprovider_a -lpostgresprovider_a
    LIBS += -lqt5keychain -lqca-qt5
    LIBS += -lgdal -lpq -lspatialite
    LIBS += -lcharset -lxml2
    LIBS += -ltasn1 -lzip -lbz2 -lproj
    LIBS += -lspatialindex -lgeos -lprotobuf-lite -lexpat -lfreexl -liconv

    RESOURCES += $$QGSQUICK_QML_DIR/QgsQuick/qgsquick.qrc
    RESOURCES += $$QGSQUICK_IMAGE_DIR/images.qrc

    QMAKE_RPATHDIR += @executable_path/../Frameworks
    QMAKE_INFO_PLIST = ios/Info.plist

    # https://doc.qt.io/qt-5.9/platform-notes-ios.html
    demoFiles.files = $$files(android/assets/qgis-data/demo-projects/*)
    demoFiles.path = qgis-data/demo-projects
    QMAKE_BUNDLE_DATA += demoFiles


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
