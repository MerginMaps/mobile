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
  
  LIBS += -lqgis_core
  LIBS += -lgdal_i
  LIBS += -lgeodiff
  LIBS += -lproj
  LIBS += -lsqlite3  
  LIBS += -lexiv2 -lexiv2-xmp
  LIBS += -lzxing
  
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
