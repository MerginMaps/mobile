ios {
    message("Building IOS")

    DEFINES += MOBILE_OS

    QGIS_QML_DIR = $${QGIS_INSTALL_PATH}/QGIS.app/Contents/MacOS/qml
    QGIS_PREFIX_PATH = $${QGIS_INSTALL_PATH}/QGIS.app/Contents/MacOS

    QGIS_QUICK_FRAMEWORK = $${QGIS_INSTALL_PATH}/QGIS.app/Contents/Frameworks/qgis_quick.framework
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

    INCLUDEPATH += $${GEODIFF_INCLUDE_DIR}
    LIBS += -L$${GEODIFF_LIB_DIR}
    LIBS += -lgeodiff

    # Disabling warnings in qgis qgswkbptr.h
    QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-shorten-64-to-32

    QGIS_LIB_DIR = $${QGIS_INSTALL_PATH}/lib
    QGIS_PROVIDER_DIR = $${QGIS_INSTALL_PATH}/QGIS.app/Contents/PlugIns/qgis
    QMAKE_TARGET_BUNDLE_PREFIX = LutraConsultingLtd
    QGSQUICK_IMAGE_DIR = $${QGIS_INSTALL_PATH}/QGIS.app/Contents/Resources/images/QgsQuick

    CONFIG -= bitcode
    CONFIG += static
    DEFINES += QT_NO_SSL

    QT += multimedia multimediawidgets location
    QTPLUGIN += qios

    LIBS += -L$${QGIS_LIB_DIR} -L$${QGIS_PROVIDER_DIR} -L$${QGIS_QML_DIR}/QgsQuick/ -L$${QGIS_QML_DIR}/../lib/
    LIBS += -lgeos -lqt5keychain -lqca-qt5 -lgdal
    LIBS += -lexpat -lcharset -lfreexl -lxml2
    LIBS += -lgdal -lproj -lspatialindex -lpq -lspatialite -lqca-qt5 -ltasn1
    LIBS += -lzip -liconv -lbz2
    LIBS += -lqgis_quick_plugin -lprotobuf-lite
    # static providers
    LIBS += -lwmsprovider_a -lpostgresprovider_a

    RESOURCES += $$QGIS_QML_DIR/QgsQuick/qgsquick.qrc
    RESOURCES += $$QGSQUICK_IMAGE_DIR/images.qrc

    QMAKE_RPATHDIR += @executable_path/../Frameworks
    QMAKE_INFO_PLIST = ios/Info.plist

    # https://doc.qt.io/qt-5.9/platform-notes-ios.html
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
        ios/iosimagepicker.mm

    DEFINES += "PURCHASING"
    DEFINES += "APPLE_PURCHASING"
    LIBS += -framework StoreKit -framework Foundation

    QMAKE_CXXFLAGS += -std=c++11
}
