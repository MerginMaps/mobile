android {
  # To build for android you need OSGeo libraries
  OSGEO4A_DIR = /home/input-sdk
  OSGEO4A_STAGE_DIR = $${OSGEO4A_DIR}
  QGIS_INSTALL_PATH = $${OSGEO4A_STAGE_DIR}/$$ANDROID_TARGET_ARCH
  QGIS_QUICK_DATA_PATH = INPUT # should be relative path
  # we try to use it as /sdcard/path and if not writable, use /storage/emulated/0/path (user home path)

}
!android {
}
