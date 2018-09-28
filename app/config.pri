BASE_DIR=$(HOME)/Projects/quick

android {
  OSGEO4A_DIR = $${BASE_DIR}/OSGeo4A
  OSGEO4A_STAGE_DIR = $${OSGEO4A_DIR}/stage
  QGIS_INSTALL_PATH = $${OSGEO4A_STAGE_DIR}/$$ANDROID_TARGET_ARCH
  QGIS_QUICK_DATA_PATH = INPUT # should be relative path
  # we try to use it as /sdcard/path and if not writable, use /storage/emulated/0/path (user home path)
}
!android {
  QGIS_INSTALL_PATH =  $${BASE_DIR}/Applications
  QGIS_QUICK_DATA_PATH = $${BASE_DIR}/input/app/android/assets/qgis-data
}
