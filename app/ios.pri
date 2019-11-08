ios {
    # https://doc.qt.io/qt-5.9/platform-notes-ios.html

    crsFiles.files = $$files(android/assets/qgis-data/resources/*.db)
    crsFiles.path = qgis-data/resources
    QMAKE_BUNDLE_DATA += crsFiles

    # app icon
    QMAKE_ASSET_CATALOGS = $$PWD/ios/Images.xcassets
    QMAKE_ASSET_CATALOGS_APP_ICON = "AppIcon"

    # launch screen
    app_launch_images.files = $$PWD/ios/launchscreen/InputScreen.xib $$files($$PWD/ios/launchscreen/*.png)
    QMAKE_BUNDLE_DATA += app_launch_images

    SOURCES += \
        ios/ioshandler.cpp \

    HEADERS += \
        ios/ioshandler.h \
        ios/iosinterface.h

    OBJECTIVE_SOURCES += \
        ios/ioshandler.mm \
        ios/iosinterface.mm
}

HEADERS += \
  $$PWD/ios/iosimagepicker.h

SOURCES += \
  $$PWD/ios/iosimagepicker.cpp
