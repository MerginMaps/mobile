android {
    # files from this folder will be added to the package
    # (and will override any default files from Qt - template is in $QTDIR/src/android)
    system($$PWD/../scripts/patch_manifest.bash $${ANDROID_VERSION_NAME} $${ANDROID_VERSION_CODE} $$PWD/android $$OUT_PWD/android_patched)
    ANDROID_PACKAGE_SOURCE_DIR = $$OUT_PWD/android_patched

    # this makes the manifest visible from Qt Creator
    DISTFILES += $$OUT_PWD/android_patched/AndroidManifest.xml

    # packaging
    ANDROID_EXTRA_LIBS += \
        $${QGIS_LIB_DIR}/libcrypto.so \
        $${QGIS_LIB_DIR}/libexpat.so \
        $${QGIS_LIB_DIR}/libgeodiff.so \
        $${QGIS_LIB_DIR}/libgeos.so \
        $${QGIS_LIB_DIR}/libgeos_c.so \
        $${QGIS_LIB_DIR}/libsqlite3.so \
        $${QGIS_LIB_DIR}/libcharset.so \
        $${QGIS_LIB_DIR}/libiconv.so \
        $${QGIS_LIB_DIR}/libfreexl.so \
        $${QGIS_LIB_DIR}/libtiff.so \
        $${QGIS_LIB_DIR}/libgdal.so \
        $${QGIS_LIB_DIR}/libproj.so \
        $${QGIS_LIB_DIR}/libspatialindex.so \
        $${QGIS_LIB_DIR}/libpq.so \
        $${QGIS_LIB_DIR}/libspatialite.so \
        $${QGIS_LIB_DIR}/libqca-qt5.so \
        $${QGIS_LIB_DIR}/libqgis_core.so \
        $${QGIS_LIB_DIR}/libqgis_quick.so \
        $${QGIS_LIB_DIR}/libqgis_native.so \
        $${QGIS_LIB_DIR}/libqt5keychain.so \
        $${QGIS_LIB_DIR}/libzip.so \
        $${QGIS_LIB_DIR}/libspatialiteprovider.so \
        $${QGIS_LIB_DIR}/libdelimitedtextprovider.so \
        $${QGIS_LIB_DIR}/libgpxprovider.so \
        $${QGIS_LIB_DIR}/libmssqlprovider.so \
        $${QGIS_LIB_DIR}/libowsprovider.so \
        $${QGIS_LIB_DIR}/libpostgresprovider.so \
        $${QGIS_LIB_DIR}/libspatialiteprovider.so \
        $${QGIS_LIB_DIR}/libssl.so \
        $${QGIS_LIB_DIR}/libwcsprovider.so \
        $${QGIS_LIB_DIR}/libwfsprovider.so \
        $${QGIS_LIB_DIR}/libwmsprovider.so \
        $$QT_LIBS_DIR/libQt5OpenGL.so \
        $$QT_LIBS_DIR/libQt5PrintSupport.so \
        $$QT_LIBS_DIR/libQt5Sensors.so \
        $$QT_LIBS_DIR/libQt5Network.so \
        $$QT_LIBS_DIR/libQt5Sql.so \
        $$QT_LIBS_DIR/libQt5Svg.so \
        $$QT_LIBS_DIR/libQt5AndroidExtras.so \


    ANDROID_EXTRA_PLUGINS += $${QGIS_QML_DIR}
}
