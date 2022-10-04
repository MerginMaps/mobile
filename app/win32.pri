win32 {
  message("Building WINDOWS")

  DEFINES += MOBILE_OS
  CONFIG += windows

  QT_LIBS_DIR = $$dirname(QMAKE_QMAKE)/../lib

  exists($${INPUT_SDK_PATH}/lib/qgis_core.lib) {
    message("Building from QGIS: $${INPUT_SDK_PATH}/lib/qgis_core.lib")
  } else {
    error("Missing QGIS Core library in $${INPUT_SDK_PATH}/lib/qgis_core.lib")
  }

  INCLUDEPATH += $${INPUT_SDK_PATH}/include
  INCLUDEPATH += $${INPUT_SDK_PATH}/include/qgis
  LIBS += -L$${INPUT_SDK_PATH}/lib
  
  # by default QMake eats linked libs if they are mentioned multiple times
  # https://stackoverflow.com/questions/18327959/qmake-how-to-link-a-library-twice/18328971
  CONFIG += no_lflags_merge

  LIBS += -lqgis_core
  LIBS += -lauthmethod_basic_a
  LIBS += -lauthmethod_esritoken_a
  LIBS += -lauthmethod_identcert_a
  LIBS += -lauthmethod_oauth2_a
  LIBS += -lauthmethod_pkcs12_a
  LIBS += -lauthmethod_pkipaths_a
  LIBS += -lprovider_arcgisfeatureserver_a
  LIBS += -lprovider_arcgismapserver_a
  LIBS += -lprovider_delimitedtext_a
  LIBS += -lprovider_spatialite_a
  LIBS += -lprovider_virtuallayer_a
  LIBS += -lprovider_wcs_a
  LIBS += -lprovider_wfs_a
  LIBS += -lprovider_wms_a
  LIBS += -lprovider_postgres_a

  # needs to be added again because of the cycling dependencies between qgis_core
  # and providers and auth methods
  LIBS += -lqgis_core

  LIBS += -lgeodiff
  LIBS += -lproj
  LIBS += -lexiv2 -lexiv2-xmp
  LIBS += -lZXing
  LIBS += -lqt6keychain -lqca
  LIBS += -lgdal -lpq -lspatialite
  LIBS += -lxml2
  LIBS += -lproj
  LIBS += -lspatialindex -lgeos -lgeos_c
  LIBS += -lprotobuf-lite -lexpat -lfreexl -lexiv2 -lexiv2-xmp
  LIBS += -lsqlite3 -lz -lzip
  LIBS += -ltiff -ljpeg
  LIBS += -lwebp
    
  QT += printsupport
  QT += widgets
  QT += multimedia
  QT += bluetooth
  DEFINES += "HAVE_BLUETOOTH"
  DEFINES += "HAVE_WIDGETS"
  
  # for exiv2 std::auto_ptr
  DEFINES += "_HAS_AUTO_PTR_ETC=1"
    
  CONFIG += c++11
  DEFINES += "_USE_MATH_DEFINES"

  # Disable: warning C4003: not enough actual parameters for macro 'QStringLiteral'
  QMAKE_CXXFLAGS += /wd4003
}
