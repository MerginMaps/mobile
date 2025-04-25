cmake_minimum_required(VERSION 3.22)

# ########################################################################################
# GLOBAL SETTINGS (call before "project()")
# ########################################################################################

# TODO automatically change with the scripts/update version script
set(MM_VERSION_MAJOR "2025")
set(MM_VERSION_MINOR "6")
set(MM_VERSION_PATCH "0")

# Note: we cannot set this for non-android build, since CMake will start looking for
# Qt6AndroidMacros.cmake
if (DEFINED ENV{INPUT_SDK_ANDROID_BASE}) # TODO android!
  # Workaround for bug https://bugreports.qt.io/browse/QTBUG-112885 in Qt6.5.0
  set(QT_NO_COLLECT_IMPORTED_TARGET_APK_DEPS TRUE)

  if (DEFINED ENV{ANDROIDAPI})
    set(ANDROIDAPI $ENV{ANDROIDAPI})
  else ()
    message(FATAL_ERROR "ANDROIDAPI not defined as env variable")
  endif ()
  # set(ANDROIDAPI "28")

  set(ANDROID_PLATFORM "android-${ANDROIDAPI}")
  set(ANDROID_NDK_PLATFORM "android-${ANDROIDAPI}")
  set(ANDROID_STL "c++_shared")

  # Target/Minimum API levels for Android, used as Input target properties
  set(INPUT_ANDROID_TARGET_SDK_VERSION "34")
  set(INPUT_ANDROID_MIN_SDK_VERSION "${ANDROIDAPI}")
  set(INPUT_ANDROID_NDK_PATH "$ENV{ANDROID_NDK_ROOT}")
  if (NOT INPUT_ANDROID_NDK_PATH)
    message(FATAL_ERROR "Set required environment variable ANDROID_NDK_ROOT.")
  endif ()
endif ()

if (IOS)
  set(ENABLE_VISIBILITY FALSE)
  set(ARCHS "arm64")
  set(PLATFORM "OS")
  set(ENABLE_ARC FALSE)
  set(ENABLE_BITCODE FALSE)
  # set(DEPLOYMENT_TARGET "16.0")
endif ()

if (APPLE)
  if (DEFINED ENV{DEPLOYMENT_TARGET})
    set(CMAKE_OSX_DEPLOYMENT_TARGET $ENV{DEPLOYMENT_TARGET})
  else ()
    message(FATAL_ERROR "DEPLOYMENT_TARGET not defined as env variable")
  endif ()
endif ()

# ########################################################################################
# PROJECT SETUP
# ########################################################################################
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(INPUT_CMAKE_TEMPLATES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_templates)
set(INPUT_SCRIPTS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scripts)

include(VersionDetect)
include(PlatformDetect)
mm_detect_platform()

project(
  Input
  VERSION ${MM_VERSION_MAJOR}.${MM_VERSION_MINOR}.${MM_VERSION_PATCH}
  DESCRIPTION "Collect. Share. Publish."
  HOMEPAGE_URL "https://merginmaps.com"
  LANGUAGES CXX
)

# ########################################################################################
# OPTIONS
# ########################################################################################

if (ANDROID OR IOS)
  set(ENABLE_TESTS_DEFAULT FALSE)
else ()
  set(ENABLE_TESTS_DEFAULT TRUE)
endif ()

if (IOS)
  set(HAVE_BLUETOOTH_DEFAULT FALSE)
else ()
  set(HAVE_BLUETOOTH_DEFAULT TRUE)
endif ()

if (DEFINED ENV{INPUT_VERSION_CODE})
  set(INPUT_VERSION_CODE_DEFAULT "$ENV{INPUT_VERSION_CODE}")
else ()
  if (ANDROID)
    set(INPUT_VERSION_CODE_DEFAULT "0000000")
  elseif (IOS)
    set(INPUT_VERSION_CODE_DEFAULT "00.00.000000")
  else ()
    set(INPUT_VERSION_CODE_DEFAULT "000000")
  endif ()
endif ()

set(COVERAGE
    FALSE
    CACHE BOOL "Compile with coverage flags"
)
set(USE_MM_SERVER_API_KEY
    FALSE
    CACHE
      BOOL
      "Determines whether we should use server API key. Decrypt file with secrets on disk if you want to use it."
)
set(QGIS_QUICK_DATA_PATH
    "INPUT"
    CACHE STRING "The internal variable pointing to the application storage folder"
)
set(IOS_USE_PRODUCTION_SIGNING
    FALSE
    CACHE
      BOOL
      "Whether to sign the ios build with production certificate. Used in CI, leave false when building locally to automatically manage signing"
)

set(ENABLE_TESTS
    ${ENABLE_TESTS_DEFAULT}
    CACHE BOOL "Whether to build tests"
)
set(HAVE_BLUETOOTH
    ${HAVE_BLUETOOTH_DEFAULT}
    CACHE BOOL "Building with bluetooth position provider"
)

set(USE_KEYCHAIN
    FALSE
    CACHE
      BOOL
      "Whether to use keychains/wallets to store credentials. If false, we use QSettings"
)

set(INPUT_VERSION_CODE
    "${INPUT_VERSION_CODE_DEFAULT}"
    CACHE STRING "Build version (for stores)"
)

mm_detect_version()
message(STATUS "Mergin Maps Mobile ${version_desc} - ${platform_desc}")

# ########################################################################################
# FIND PACKAGES
# ########################################################################################

if (WIN)
  find_package(WMF REQUIRED)
endif ()

find_package(
  Qt6
  COMPONENTS Quick
             Qml
             Xml
             Concurrent
             Positioning
             Sensors
             QuickControls2
             Network
             NetworkAuth
             Svg
             Sql
             Core
             Core5Compat
             Multimedia
             Widgets
  REQUIRED
)

if (NOT IOS)
  # https://doc.qt.io/qt-6/qtprintsupport-index.html Qt Print Support is not available on
  # iOS.
  find_package(
    Qt6
    COMPONENTS PrintSupport
    REQUIRED
  )
endif ()

if (NOT LNX)
  # find_package(Charset CONFIG REQUIRED)
  find_package(Iconv REQUIRED)
endif ()

find_package(ZLIB REQUIRED)
# find_package(Bz2 CONFIG REQUIRED) find_package(bzip2 CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
# find_package(Inih CONFIG REQUIRED)
find_package(exiv2 CONFIG REQUIRED)
find_package(expat CONFIG REQUIRED)
# find_package(freexl CONFIG REQUIRED)
find_package(GDAL CONFIG REQUIRED)
find_package(Geodiff CONFIG REQUIRED)
find_package(GEOS CONFIG REQUIRED)
# find_package(Jpeg CONFIG REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(PROJ CONFIG REQUIRED)
find_package(Protobuf REQUIRED)
find_package(QGIS REQUIRED)
find_package(Qca CONFIG REQUIRED)
find_package(Qt6Keychain CONFIG REQUIRED)
find_package(Spatialindex REQUIRED)
find_package(Spatialite REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
# find_package(Tiff CONFIG REQUIRED)
find_package(WebP CONFIG REQUIRED)
# find_package(Xml2 CONFIG REQUIRED)
find_package(ZXing CONFIG REQUIRED)
find_package(libzip CONFIG REQUIRED)
find_package(OpenSSL CONFIG REQUIRED)
find_package(GeoTIFF CONFIG REQUIRED)
# find_package(Lzma CONFIG REQUIRED) find_package(Zstd CONFIG REQUIRED)
find_package(unofficial-minizip CONFIG REQUIRED)
# find_package(Utf8 CONFIG REQUIRED) find_package(Absl CONFIG REQUIRED)

if (IOS OR MACOS)
  find_package(AppleFrameworks REQUIRED)
  find_package(Tasn1 REQUIRED)
endif ()

if (HAVE_BLUETOOTH)
  find_package(
    Qt6
    COMPONENTS Bluetooth
    REQUIRED
  )
endif ()

if (ENABLE_TESTS)
  find_package(
    Qt6
    COMPONENTS Test
    REQUIRED
  )
endif ()

# ########################################################################################
# GLOBAL SETUP
# ########################################################################################

qt_standard_project_setup()

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if (ENABLE_TESTS)
  enable_testing()
endif ()

# ########################################################################################
# GLOBAL DEFINES
# ########################################################################################
# Exiv still uses std::auto_ptr
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_definitions(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
endif ()

if (WIN)
  add_compile_definitions(_HAS_AUTO_PTR_ETC=1)
  add_compile_definitions(_USE_MATH_DEFINES)
endif ()

if (IOS OR ANDROID)
  add_compile_definitions(MOBILE_OS)
else ()
  add_compile_definitions(DESKTOP_OS)
endif ()

if (IOS)
  # Using Security.SecureTransport for SSL
  add_compile_definitions(QT_NO_SSL)
endif ()

if (IOS)
  add_compile_definitions(QT_NO_PRINTER)
endif ()

# This is for QGIS defined, do not move to inputconfig.h
add_compile_definitions(HAVE_WIDGETS)

# QGIS defines
# add_compile_definitions("$<$<CONFIG:DEBUG>:QGIS_PREFIX_PATH=${QGIS_PREFIX_PATH}>")
add_compile_definitions("$<$<CONFIG:DEBUG>:QGIS_QUICK_EXPAND_TEST_DATA>")

# Generate inputconfig.h
if (ENABLE_TESTS)
  set(INPUT_TEST TRUE)
  file(COPY test/test_data DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/)
  set(TEST_DATA_DIR "${CMAKE_CURRENT_BINARY_DIR}/test/test_data")
endif ()

configure_file(
  ${CMAKE_SOURCE_DIR}/cmake_templates/inputconfig.h.in ${CMAKE_BINARY_DIR}/inputconfig.h
)
include_directories(${CMAKE_BINARY_DIR})

# ########################################################################################
# GLOBAL COMPILE FLAGS
# ########################################################################################

if (COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif ()

if (MACOS)
  # lot of errors like: This was likely caused by different translation units being
  # compiled with different visibility settings
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden -fvisibility=hidden"
  )
endif ()

if (NOT WIN)
  # Hundreds of warnings from QGIS core warning: 'Type' is deprecated: Use QMetaType::Type
  # instead
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif ()

if (WIN)
  # Disable: warning C4003: not enough actual parameters for macro 'QStringLiteral'
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4003")
endif ()

if (IOS)
  # Disabling warnings in qgis qgswkbptr.h
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-shorten-64-to-32")
endif ()

# ########################################################################################
# RESOURCES
# ########################################################################################
# TODO - this would need to rething the approach so the assets are in binary folder
message(STATUS "Copying proj and qgis resources to android/assets/qgis-data")
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/app/android/assets/qgis-data/resources)
file(COPY ${QGIS_RESOURCE_DIR}/qgis.db
     DESTINATION ${CMAKE_SOURCE_DIR}/app/android/assets/qgis-data/resources/
)
file(COPY ${QGIS_RESOURCE_DIR}/srs.db
     DESTINATION ${CMAKE_SOURCE_DIR}/app/android/assets/qgis-data/resources/
)
file(
  COPY ${PROJ_DIR}
  DESTINATION ${CMAKE_SOURCE_DIR}/app/android/assets/qgis-data/
  PATTERN "*.cmake" EXCLUDE
  PATTERN "vcpkg*" EXCLUDE
)

# ########################################################################################
# SUBDIRECTORIES
# ########################################################################################

add_subdirectory(core)
add_subdirectory(app)
if (ENABLE_TESTS)
  add_subdirectory(test)
endif ()

# ########################################################################################
# INSTALL
# ########################################################################################

if (WIN
    OR LNX
    OR MACOS
)
  # qt_add_executable on Android automatically creates target for apk/aab on IOS we use
  # xcodebuild to build ipa
  include(InstallStep)
endif ()
