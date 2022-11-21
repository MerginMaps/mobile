cmake_minimum_required(VERSION 3.22)

#TODO automatically change with the scripts/update version script
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

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# APPLE is both iOS and macOS
if (APPLE AND NOT IOS)
    set(MACOS TRUE)
endif()

if(UNIX AND NOT ANDROID AND NOT APPLE)
    set(LINUX TRUE)
endif()   

if(MSCV)
    set(WIN TRUE)
endif()

if (MACOS)
    set(platform "macOS")
elseif(IOS)
    set(platform "iOS")
elseif(ANDROID)
    set(platform "android")
elseif(WIN)
    set(platform "windows")
elseif(LINUX)
    set(platform "linux")
else()
    message(FATAL_ERROR "unknown target platform")
endif()

message(STATUS "Mergin Maps Input ${MM_VERSION_MAJOR}.${MM_VERSION_MINOR}.${MM_VERSION_PATCH} - ${platform}")

# TODO MACOS_DEPLOYMENT_TARGET for macos
# TODO ANDROID_TARGET_SDK_VERSION = 31
# TODO ANDROID_MIN_SDK_VERSION = 24
######################################################################
## OPTIONS
######################################################################

if(ANDROID OR IOS)
  set(ENABLE_TESTS_DEFAULT FALSE)
else()
  set(ENABLE_TESTS_DEFAULT TRUE)
endif()

if(IOS)
  set(HAVE_BLUETOOTH_DEFAULT FALSE)
else()
  set(HAVE_BLUETOOTH_DEFAULT TRUE)
endif()

if(IOS)
  set(HAVE_PURCHASING_DEFAULT TRUE) 
  set(HAVE_APPLE_PURCHASING_DEFAULT TRUE)
else()
  set(HAVE_PURCHASING_DEFAULT ${ENABLE_TESTS_DEFAULT})
  set(HAVE_APPLE_PURCHASING_DEFAULT FALSE)
endif()

set(COVERAGE FALSE CACHE BOOL "Compile with coverage flags")
set(USE_SERVER_API_KEY TRUE CACHE BOOL "Determines whether we should use server API key. Decrypt file with secrets on disk if you want to use it.")
set(QGIS_QUICK_DATA_PATH "INPUT" CACHE STRING "The internal variable pointing to the application storage folder") 
set(INPUT_SDK_PATH "" CACHE PATH "Path to the Mergin Maps Input SDK on host machine" )
set(ENABLE_TESTS ${ENABLE_TESTS_DEFAULT} CACHE BOOL "Whether to build tests")
set(QT6_VERSION ${QT_VERSION_DEFAULT} CACHE STRING "QT6 version to use")
set(HAVE_BLUETOOTH ${HAVE_BLUETOOTH_DEFAULT} CACHE STRING "Building with bluetooth position provider")
set(HAVE_PURCHASING ${HAVE_PURCHASING_DEFAULT} CACHE STRING "Build with purchasing (e.g. for test of purchasing GUI)")
set(HAVE_APPLE_PURCHASING ${HAVE_APPLE_PURCHASING_DEFAULT} CACHE STRING "Building with Apple's StoreKit support")

######################################################################
## FIND PACKAGES
######################################################################

if(ANDROID)
  # allow libraries outside of SDK/NDK directory
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
  set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
endif()

# TODO: OpenGl needed?
find_package(Qt6 ${QT6_VERSION} COMPONENTS Quick Qml Xml Concurrent Positioning Sensors QuickControls2 Network Svg Sql Core Core5Compat REQUIRED)
find_package(Qt6 ${QT6_VERSION} COMPONENTS Multimedia Widgets REQUIRED)

if (NOT IOS)
    # https://doc.qt.io/qt-6/qtprintsupport-index.html
    # Qt Print Support is not available on iOS.
  find_package(Qt6 ${QT6_VERSION} COMPONENTS PrintSupport REQUIRED)
endif()

if (MACOS OR IOS OR ANDROID)
    find_package(Charset REQUIRED)
    find_package(Iconv REQUIRED)
endif()

if (MACOS OR IOS)
  find_package(Bz2 REQUIRED)
endif()

find_package(Curl REQUIRED)
find_package(Exiv2 REQUIRED)
find_package(Expat REQUIRED)
find_package(Freexl REQUIRED)
find_package(Gdal REQUIRED)
find_package(Geodiff REQUIRED)
find_package(Geos REQUIRED)
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
find_package(Tiff REQUIRED)
find_package(WebP REQUIRED)
if (NOT ANDROID)
  find_package(Xml2 REQUIRED)
endif()
find_package(ZLIB REQUIRED)
find_package(ZXing REQUIRED)
find_package(Zip REQUIRED)

if (ANDROID)
  find_package(OpenSSL REQUIRED)
endif()

if (APPLE)
  find_package(AppleFrameworks REQUIRED)
endif()

if (NOT LINUX)
  find_package(Tasn1 REQUIRED)
endif()

if (HAVE_BLUETOOTH)
  find_package(Qt6 ${QT6_VERSION} COMPONENTS Bluetooth REQUIRED)
endif()

if (ENABLE_TESTS)
  find_package(Qt6 ${QT6_VERSION} COMPONENTS Test REQUIRED)
endif()

######################################################################
## GLOBAL SETUP
######################################################################

qt_standard_project_setup()

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_AUTOMOC ON)

if (ENABLE_TESTS)
  enable_testing()
endif()

######################################################################
## GLOBAL DEFINES
######################################################################

# TODO move most of the C++ defines to generated header file!

# Exiv still uses std::auto_ptr
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_definitions(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
endif()
if (MSVC)
  add_compile_definitions(_HAS_AUTO_PTR_ETC=1)
endif()

if (COVERAGE)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} --coverage" )
    set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} --coverage" )
endif()

if (HAVE_BLUETOOTH)
  add_compile_definitions(HAVE_BLUETOOTH)
endif()

if (HAVE_PURCHASING)
    add_compile_definitions(PURCHASING)
endif()

if (HAVE_APPLE_PURCHASING)
    add_compile_definitions(APPLE_PURCHASING)
endif()
    
if(IOS OR ANDROID)
  add_compile_definitions(MOBILE_OS)
else()
  add_compile_definitions(DESKTOP_OS)
endif()

if(IOS)
  add_compile_definitions(QT_NO_SSL)
  add_compile_definitions(QT_NO_PRINTER)
endif()

add_compile_definitions(HAVE_WIDGETS)
add_compile_definitions(INPUT_APP)

add_compile_definitions(QGIS_QUICK_DATA_PATH=${QGIS_QUICK_DATA_PATH})

#TODO also for RelWithDebug?
add_compile_definitions("$<$<CONFIG:DEBUG>:QGIS_PREFIX_PATH=${QGIS_PREFIX_PATH}>")
add_compile_definitions("$<$<CONFIG:DEBUG>:QGIS_QUICK_EXPAND_TEST_DATA>")

if (ENABLE_TESTS)
  add_compile_definitions(INPUT_TEST)
  set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../test/test_data")
  add_compile_definitions(TEST_DATA_DIR=\"${TEST_DATA_DIR}\" )
endif()

######################################################################
## GLOBAL COMPILE FLAGS
######################################################################

if (MACOS)
    # lot of errors like: This was likely caused by different translation units being compiled with different visibility settings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden -fvisibility=hidden")
endif()

if(NOT MSVC) 
  # Hundreds of warnings from QGIS core
  # warning: 'Type' is deprecated: Use QMetaType::Type instead
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif()

if (IOS)
    # Disabling warnings in qgis qgswkbptr.h
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-shorten-64-to-32")  
endif()


######################################################################
## SUBDIRECTORIES
######################################################################

add_subdirectory(core)
add_subdirectory(app)

######################################################################
## INSTALL
######################################################################

include(InstallStep)
