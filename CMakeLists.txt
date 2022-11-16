cmake_minimum_required(VERSION 3.22)

set(MM_VERSION_MAJOR "2")
set(MM_VERSION_MINOR "0")
set(MM_VERSION_PATCH "0")
set(QT_VERSION_DEFAULT "6.4")

project(
  merginmaps
  VERSION ${MM_VERSION_MAJOR}.${MM_VERSION_MINOR}.${MM_VERSION_PATCH}
  DESCRIPTION "The easiest way to take your QGIS projects into the field"
  HOMEPAGE_URL "https://merginmaps.com"
  LANGUAGES CXX
)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Exiv still uses std::auto_ptr
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_definitions(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
endif()
if (MSVC)
  add_compile_definitions(_HAS_AUTO_PTR_ETC=1)
endif()

set(COVERAGE FALSE CACHE BOOL "Compile with coverage flags")
set(USE_SERVER_API_KEY TRUE CACHE BOOL "Determines whether we should use server API key. Decrypt file with secrets on disk if you want to use it.")
set(QGIS_QUICK_DATA_PATH "INPUT" CACHE STRING "The internal variable pointing to the application storage folder") 
set(INPUT_SDK_PATH "" CACHE PATH "Path to the Mergin Maps Input SDK on host machine" )
set(ENABLE_TESTS TRUE CACHE BOOL "Whether to build tests")
set(QT6_VERSION ${QT_VERSION_DEFAULT} CACHE STRING "QT6 version to use")
set(HAVE_BLUETOOTH TRUE CACHE STRING "Building with bluetooth position provider")

find_package(Qt6 ${QT6_VERSION} COMPONENTS Quick Qml Xml Concurrent Positioning Sensors QuickControls2 Network Svg Sql OpenGl Core Core5Compat REQUIRED)
find_package(Qt6 ${QT6_VERSION} COMPONENTS Multimedia Widgets REQUIRED)

if (COVERAGE)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} --coverage" )
    set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} --coverage" )
endif()

if (HAVE_BLUETOOTH)
  find_package(Qt6 ${QT6_VERSION} COMPONENTS Bluetooth REQUIRED)
  add_compile_definitions(HAVE_BLUETOOTH)
endif()

if (NOT IOS)
    # https://doc.qt.io/qt-6/qtprintsupport-index.html
    # Qt Print Support is not available on iOS.
  find_package(Qt6 ${QT6_VERSION} COMPONENTS PrintSupport REQUIRED)
endif()

find_package(Bz2 REQUIRED)
find_package(Charset REQUIRED)
find_package(Curl REQUIRED)
find_package(Exiv2 REQUIRED)
find_package(Expat REQUIRED)
find_package(Freexl REQUIRED)
find_package(Gdal REQUIRED)
find_package(Geodiff REQUIRED)
find_package(Geos REQUIRED)
find_package(Iconv REQUIRED)
find_package(Jpeg REQUIRED)
find_package(Jpeg REQUIRED)
find_package(Pq REQUIRED)
find_package(Proj REQUIRED)
find_package(Protobuf REQUIRED)
find_package(QGIS REQUIRED)
find_package(Qca REQUIRED)
find_package(Qtkeychain REQUIRED)
find_package(Spatialindex REQUIRED)
find_package(Spatialite REQUIRED)
find_package(Sqlite3 REQUIRED)
find_package(Tasn1 REQUIRED)
find_package(Tiff REQUIRED)
find_package(WebP REQUIRED)
find_package(Xml2 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(ZXing REQUIRED)
find_package(Zip REQUIRED)

if (APPLE)
  find_package(AppleFrameworks REQUIRED)
endif()


if(NOT MSVC) 
  # Hundreds of warnings from QGIS core
  # warning: 'Type' is deprecated: Use QMetaType::Type instead
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif()

if (APPLE)
    add_compile_definitions(DESKTOP_OS)
    add_compile_definitions(PURCHASING) # PURCHASING stuff (only testing)
    add_compile_definitions(HAVE_WIDGETS)
else()
    add_compile_definitions(MOBILE_OS)
endif()

add_compile_definitions(INPUT_APP)

# DEFINES += "QGIS_QUICK_DATA_PATH=$${QGIS_QUICK_DATA_PATH}"
add_compile_definitions(QGIS_QUICK_DATA_PATH=${QGIS_QUICK_DATA_PATH})


# CONFIG(debug, debug|release) {
#  DEFINES += "QGIS_PREFIX_PATH=$${QGIS_PREFIX_PATH}"
#  DEFINES += "QGIS_QUICK_EXPAND_TEST_DATA"
#}
#TODO debug|release?
add_compile_definitions("$<$<CONFIG:DEBUG>:-DQGIS_PREFIX_PATH=${QGIS_PREFIX_PATH}>")
add_compile_definitions("$<$<CONFIG:DEBUG>:-DQGIS_QUICK_EXPAND_TEST_DATA>")

if (ENABLE_TESTS)
  enable_testing()
  find_package(Qt6 ${QT6_VERSION} COMPONENTS Test REQUIRED)
  add_compile_definitions(INPUT_TEST)
  
  # contains(DEFINES, INPUT_TEST) {
  #   TEST_DATA_DIR="$$PWD/../test/test_data"
  #  DEFINES += "TEST_DATA_DIR=\\\"$${TEST_DATA_DIR}\\\""
  # }
  set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../test/test_data")
  add_compile_definitions(TEST_DATA_DIR=\"${TEST_DATA_DIR}\" )
endif()

add_subdirectory(core)
add_subdirectory(app)
