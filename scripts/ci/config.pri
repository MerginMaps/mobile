android {
  # To build for android you need OSGEO4A
  OSGEO4A_DIR = /home/osgeo4a
  OSGEO4A_STAGE_DIR = $${OSGEO4A_DIR}/stage
  QGIS_INSTALL_PATH = $${OSGEO4A_STAGE_DIR}/$$ANDROID_TARGET_ARCH
  QGIS_QUICK_DATA_PATH = INPUT # should be relative path
  # we try to use it as /sdcard/path and if not writable, use /storage/emulated/0/path (user home path)

}
!android {
}
