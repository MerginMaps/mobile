ios {
    # https://doc.qt.io/qt-5.9/platform-notes-ios.html

    crsFiles.files = $$files(android/assets/qgis-data/resources/*.db)
    crsFiles.path = Documents/qgis-data/resources
    QMAKE_BUNDLE_DATA += crsFiles
}
