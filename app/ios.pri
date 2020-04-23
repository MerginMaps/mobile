ios {
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
        ios/iosviewdelegate.h \

    OBJECTIVE_SOURCES += \
        ios/iosinterface.mm \
        ios/iosviewdelegate.mm \
        ios/iosimagepicker.mm
}
