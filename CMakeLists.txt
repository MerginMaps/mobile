cmake_minimum_required(VERSION 3.22)

# ########################################################################################
# GLOBAL SETTINGS (call before "project()")
# ########################################################################################

# TODO automatically change with the scripts/update version script
set(MM_VERSION_MAJOR "2025")
set(MM_VERSION_MINOR "6")
set(MM_VERSION_PATCH "0")
set(QT_VERSION_DEFAULT "6.8.3")

# Note: we cannot set this for non-android build, since CMake will start looking for
# Qt6AndroidMacros.cmake
if (DEFINED ENV{INPUT_SDK_ANDROID_BASE})
  # Workaround for bug https://bugreports.qt.io/browse/QTBUG-112885 in Qt6.5.0
  set(QT_NO_COLLECT_IMPORTED_TARGET_APK_DEPS TRUE)

  set(ANDROIDAPI "28")
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
  set(DEPLOYMENT_TARGET "16.0")
endif ()

if (APPLE
    AND NOT IOS
    AND NOT DEFINED ENV{INPUT_SDK_ANDROID_BASE}
)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15.0")
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
  DESCRIPTION "The easiest way to take your QGIS projects into the field"
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

# on android in multi-ABI build, command line variables are NOT passed to
# ExternalProject_Add called by QT. Therefore we need to pass the variables through ENV
if (ANDROID)
  if (NOT DEFINED ENV{INPUT_SDK_ANDROID_BASE})
    message(
      FATAL_ERROR
        "On Android, you need to define INPUT_SDK_ANDROID_BASE as environment variable. Clean build directory and run cmake again!"
    )
  else ()
    set(INPUT_SDK_PATH_DEFAULT $ENV{INPUT_SDK_ANDROID_BASE})
  endif ()
else ()
  set(INPUT_SDK_PATH_DEFAULT "")
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
set(INPUT_SDK_PATH
    ${INPUT_SDK_PATH_DEFAULT}
    CACHE
      PATH
      "Path to the Mergin Maps Mobile-SDK on host machine; on android define ENV variable instead and without the ABI suffix (armeabi-v7a, arm64-v8a)"
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

set(QT6_VERSION
    ${QT_VERSION_DEFAULT}
    CACHE STRING "QT6 version to use"
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

if (ANDROID)
  set(INPUT_SDK_PATH_ANDROID_ABI_armeabi-v7a ${INPUT_SDK_PATH}/arm-android)

  if (NOT EXISTS ${INPUT_SDK_PATH_ANDROID_ABI_armeabi-v7a})
    message(WARNING "INPUT_SDK_PATH arm-android directory does not exist!")
  endif ()

  set(INPUT_SDK_PATH_ANDROID_ABI_arm64-v8a ${INPUT_SDK_PATH}/arm64-android)
  if (NOT EXISTS ${INPUT_SDK_PATH_ANDROID_ABI_arm64-v8a})
    message(WARNING "INPUT_SDK_PATH arm64-android directory does not exist!")
  endif ()

  set(INPUT_SDK_PATH_MULTI ${INPUT_SDK_PATH_ANDROID_ABI_${ANDROID_ABI}})
  # allow libraries outside of SDK/NDK directory
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
  set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
else ()
  set(INPUT_SDK_PATH_MULTI ${INPUT_SDK_PATH})
endif ()

find_package(
  Qt6 ${QT6_VERSION}
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
  REQUIRED
)
find_package(
  Qt6 ${QT6_VERSION}
  COMPONENTS Multimedia Widgets
  REQUIRED
)

if (NOT IOS)
  # https://doc.qt.io/qt-6/qtprintsupport-index.html Qt Print Support is not available on
  # iOS.
  find_package(
    Qt6 ${QT6_VERSION}
    COMPONENTS PrintSupport
    REQUIRED
  )
endif ()

if (NOT LNX)
  find_package(Charset REQUIRED)
  find_package(Iconv REQUIRED)
endif ()

find_package(ZLIB REQUIRED)
find_package(Bz2 REQUIRED)
find_package(Curl REQUIRED)
find_package(Inih REQUIRED)
find_package(Exiv2 REQUIRED)
find_package(Expat REQUIRED)
find_package(Freexl REQUIRED)
find_package(Gdal REQUIRED)
find_package(Geodiff REQUIRED)
find_package(Geos REQUIRED)
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
find_package(Xml2 REQUIRED)
find_package(ZXing REQUIRED)
find_package(Zip REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Geotiff REQUIRED)
find_package(Lzma REQUIRED)
find_package(Zstd REQUIRED)
find_package(MiniZip REQUIRED)
find_package(Utf8 REQUIRED)
find_package(Absl REQUIRED QUIET)

if (IOS OR MACOS)
  find_package(AppleFrameworks REQUIRED)
  find_package(Tasn1 REQUIRED)
endif ()

if (HAVE_BLUETOOTH)
  find_package(
    Qt6 ${QT6_VERSION}
    COMPONENTS Bluetooth
    REQUIRED
  )
endif ()

if (ENABLE_TESTS)
  find_package(
    Qt6 ${QT6_VERSION}
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
