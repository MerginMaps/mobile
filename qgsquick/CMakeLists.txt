# QgsQuick
# Copyright (C) 2020 Peter Petrik (zilolv at gmail dot com)

CMAKE_MINIMUM_REQUIRED(VERSION 3.9)
PROJECT(QgsQuick)

SET (CMAKE_CXX_STANDARD 11)

#############################################################
# global vars

SET (USE_QGIS_BUILD_DIR FALSE CACHE BOOL "Use QGIS from build directory")
SET (QGIS_CMAKE_PATH "" CACHE PATH "Path to QGIS cmake directory")
SET (QGIS_INSTALL_PATH "" CACHE PATH "QGIS install directory, only used when USE_QGIS_BUILD_DIR=FALSE")
SET (QGIS_BUILD_DIR "" CACHE PATH "QGIS build directory, only used when USE_QGIS_BUILD_DIR=TRUE")
SET (QGIS_SRC_DIR "" CACHE PATH "QGIS source directory, only used when USE_QGIS_BUILD_DIR=TRUE")
SET (FORCE_STATIC_LIBS FALSE CACHE BOOL "Determines whether libraries should be static only")
SET (QGIS_VERSION_MAJOR "3" CACHE STRING "QGIS VERSION STRING")
SET (QGIS_VERSION_MINOR "17" CACHE STRING "QGIS VERSION STRING")
SET (QGIS_VERSION_PATCH "0" CACHE STRING "QGIS VERSION STRING")
SET (ANDROID_INPUT_SDK_PATH "" CACHE STRING "stage path for android SDK stage folder")
SET (IOS_INPUT_SDK_PATH "" CACHE STRING "stage path for ios SDK stage folder")
SET (IOS_SYSROOT_PATH "" CACHE STRING "ios sysroot folder")
SET (ENABLE_TESTS FALSE CACHE STRING "enable testing")

IF (QGIS_SRC_DIR AND NOT QGIS_CMAKE_PATH)
    SET(QGIS_CMAKE_PATH "${QGIS_SRC_DIR}/cmake" FORCE)
    MESSAGE("Using CMAKE path from QGIS_SRC_DIR: ${QGIS_CMAKE_PATH}")
ENDIF (QGIS_SRC_DIR AND NOT QGIS_CMAKE_PATH)

IF (APPLE)
  SET (QGIS_MAC_DEPS_DIR "" CACHE PATH "Path to QGIS Mac custom dependencies directory")
  IF ( QGIS_MAC_DEPS_DIR )
    SET(ENV{LIB_DIR} ${QGIS_MAC_DEPS_DIR})
    LIST(APPEND CMAKE_PREFIX_PATH ${QGIS_MAC_DEPS_DIR})
  ENDIF()
ENDIF()

SET(CMAKE_MODULE_PATH ${QGIS_CMAKE_PATH} ${CMAKE_MODULE_PATH})

IF (WIN32)
  IF (MSVC)
    ADD_DEFINITIONS(-D_USE_MATH_DEFINES)
  ENDIF (MSVC)
ENDIF (WIN32)

#############################################################
# Find Packages
INCLUDE(GenerateExportHeader)
SET(CMAKE_SOURCE_DIR_MINE ${CMAKE_SOURCE_DIR})
SET(CMAKE_SOURCE_DIR ${QGIS_CMAKE_PATH}/..)

IF (ANDROID)
    MESSAGE("Using ANDROID-INPUT-SDK: ${ANDROID_INPUT_SDK_PATH}")
    MESSAGE("ANDROID ARCH: ${ANDROID_ABI}")

    SET(QGIS_CMAKE_PATH "${ANDROID_INPUT_SDK_PATH}/cmake")
    SET(GDAL_VERSION "3.1.3")
    SET(GDAL_CONFIG "${ANDROID_INPUT_SDK_PATH}/bin/gdal-config")
    SET(GDAL_INCLUDE_DIR "${ANDROID_INPUT_SDK_PATH}/include")
    SET(GDAL_LIBRARY "${ANDROID_INPUT_SDK_PATH}/lib/libgdal.so")
    SET(GEOS_CONFIG "${ANDROID_INPUT_SDK_PATH}/bin/geos-config")
    SET(GEOS_INCLUDE_DIR "${ANDROID_INPUT_SDK_PATH}/include")
    SET(GEOS_LIBRARY "${ANDROID_INPUT_SDK_PATH}/lib/libgeos_c.so")
    SET(SQLITE3_INCLUDE_DIR "${ANDROID_INPUT_SDK_PATH}/include")
    SET(SQLITE3_LIBRARY "${ANDROID_INPUT_SDK_PATH}/lib/libsqlite3.so")
    SET(QCA_INCLUDE_DIR "${ANDROID_INPUT_SDK_PATH}/include/Qca-qt5/QtCrypto")
    SET(QCA_LIBRARY "${ANDROID_INPUT_SDK_PATH}/lib/libqca-qt5_${ANDROID_ABI}.so")
    SET(QTKEYCHAIN_INCLUDE_DIR "${ANDROID_INPUT_SDK_PATH}/include/qt5keychain")
    SET(QTKEYCHAIN_LIBRARY "${ANDROID_INPUT_SDK_PATH}/lib/libqt5keychain_${ANDROID_ABI}.so")
    SET(EXPAT_INCLUDE_DIR "${ANDROID_INPUT_SDK_PATH}/include")
    SET(EXPAT_LIBRARY "${ANDROID_INPUT_SDK_PATH}/lib/libexpat.so")
    SET(SPATIALINDEX_LIBRARY "${ANDROID_INPUT_SDK_PATH}/lib/libspatialindex.so")
    SET(NATIVE_CRSSYNC_BIN "/usr/bin/true")

ELSE (ANDROID)
    IF (IOS)
        MESSAGE("Using IOS-INPUT-SDK: ${IOS_INPUT_SDK_PATH}")
        MESSAGE("Using IOS-ROOT: ${IOS_SYSROOT_PATH}")

        SET(CMAKE_FIND_FRAMEWORK "NEVER" FORCE)
        SET(GDAL_CONFIG "${IOS_INPUT_SDK_PATH}/bin/gdal-config")
        SET(GDAL_CONFIG_PREFER_FWTOOLS_PAT "/bin_safe")
        SET(GDAL_CONFIG_PREFER_PATH "${IOS_INPUT_SDK_PATH}/bin")
        SET(GDAL_INCLUDE_DIR "${IOS_INPUT_SDK_PATH}/include")
        SET(GDAL_LIBRARY "${IOS_INPUT_SDK_PATH}/lib/libgdal.a")
        SET(GDAL_VERSION "3.1.3")
        SET(GEOS_CONFIG "${IOS_INPUT_SDK_PATH}/bin/geos-config")
        SET(GEOS_CONFIG_PREFER_PATH "${IOS_INPUT_SDK_PATH}/bin")
        SET(GEOS_INCLUDE_DIR "${IOS_INPUT_SDK_PATH}/include")
        SET(GEOS_LIBRARY "${IOS_INPUT_SDK_PATH}/lib/libgeos_c.a")
        SET(GEOS_LIB_NAME_WITH_PREFIX "-lgeos_c")
        SET(GEOS_VERSION "3.7.2")
        SET(QCA_INCLUDE_DIR "${IOS_INPUT_SDK_PATH}/include/QtCrypto")
        SET(QCA_LIBRARY "${IOS_INPUT_SDK_PATH}/lib/libqca-qt5.a")
        SET(QCA_VERSION_STR "2.1.0")
        SET(PROJ_INCLUDE_DIR "${IOS_INPUT_SDK_PATH}/include")
        SET(PROJ_LIBRARY "${IOS_INPUT_SDK_PATH}/lib/libproj.a")
        SET(LIBTASN1_INCLUDE_DIR "${IOS_INPUT_SDK_PATH}/include")
        SET(LIBTASN1_LIBRARY "${IOS_INPUT_SDK_PATH}/lib/libtasn1.a")
        SET(LIBZIP_INCLUDE_DIR "${IOS_INPUT_SDK_PATH}/include")
        SET(LIBZIP_CONF_INCLUDE_DIR "${IOS_INPUT_SDK_PATH}/include")
        SET(LIBZIP_LIBRARY "${IOS_INPUT_SDK_PATH}/lib/libzip.a")
        SET(ICONV_INCLUDE_DIR "${IOS_SYSROOT_PATH}")
        SET(ICONV_LIBRARY "${IOS_SYSROOT_PATH}/usr/lib/libiconv.tbd")
        SET(SQLITE3_INCLUDE_DIR "${IOS_SYSROOT_PATH}")
        SET(SQLITE3_LIBRARY "${IOS_SYSROOT_PATH}/usr/lib/libsqlite3.tbd")
        SET(POSTGRES_CONFIG)
        SET(POSTGRES_CONFIG_PREFER_PATH)
        SET(POSTGRES_INCLUDE_DIR "${IOS_INPUT_SDK_PATH}/include")
        SET(POSTGRES_LIBRARY "${IOS_INPUT_SDK_PATH}/lib/libpq.a")
        SET(QTKEYCHAIN_INCLUDE_DIR "${IOS_INPUT_SDK_PATH}/include/qt5keychain")
        SET(QTKEYCHAIN_LIBRARY "${IOS_INPUT_SDK_PATH}/lib/libqt5keychain.a")
        SET(SPATIALINDEX_INCLUDE_DIR "${IOS_INPUT_SDK_PATH}/include/spatialindex")
        SET(SPATIALINDEX_LIBRARY "${IOS_INPUT_SDK_PATH}/lib/libspatialindex.a")
        SET(SPATIALITE_INCLUDE_DIR "${IOS_INPUT_SDK_PATH}/include")
        SET(SPATIALITE_LIBRARY "${IOS_INPUT_SDK_PATH}/lib/libspatialite.a")
        SET(EXPAT_INCLUDE_DIR "${IOS_INPUT_SDK_PATH}/include")
        SET(EXPAT_LIBRARY "${IOS_INPUT_SDK_PATH}/lib/libexpat.a")
        SET(CMAKE_DISABLE_FIND_PACKAGE_QtQmlTools TRUE)
        SET(LIBTIFF_LIBRARY "${IOS_INPUT_SDK_PATH}/lib/libtiff.a")
        SET(LIBTIFFXX_LIBRARY "${IOS_INPUT_SDK_PATH}/lib/libtiffxx.a")
        SET(FREEXL_LIBRARY "${IOS_INPUT_SDK_PATH}/lib/libfreexl.a")
        SET(CHARSET_LIBRARY "${IOS_INPUT_SDK_PATH}/lib/libcharset.a")
        SET(GEOSCXX_LIBRARY "${IOS_INPUT_SDK_PATH}/lib/libgeos.a")
    ENDIF (IOS)
ENDIF (ANDROID)

FIND_PACKAGE(Proj)
FIND_PACKAGE(GEOS)
FIND_PACKAGE(GDAL)
FIND_PACKAGE(Expat REQUIRED)
FIND_PACKAGE(Spatialindex REQUIRED)
FIND_PACKAGE(LibZip REQUIRED)
FIND_PACKAGE(Sqlite3)
FIND_PACKAGE(SpatiaLite REQUIRED)
FIND_PACKAGE(QtKeychain REQUIRED)
FIND_PACKAGE(QCA REQUIRED)
SET(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR_MINE})

SET(QT_MIN_VERSION 5.9.0)
FIND_PACKAGE(Qt5 COMPONENTS Core Gui Widgets Network Xml Svg Concurrent Test UiTools Sql REQUIRED)
IF (NOT IOS)
  FIND_PACKAGE(Qt5 COMPONENTS PrintSupport REQUIRED)
ELSE()
  ADD_DEFINITIONS(-DQT_NO_PRINTER)
ENDIF()
FIND_PACKAGE(Qt5 COMPONENTS Positioning)
IF (APPLE)
  FIND_PACKAGE(Qt5 COMPONENTS MacExtras REQUIRED)
ENDIF()

FIND_PACKAGE(Qt5 COMPONENTS Qml Quick REQUIRED)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    FIND_PACKAGE(Qt5 COMPONENTS AndroidExtras)
ELSE()
    FIND_PACKAGE(QtQmlTools)
ENDIF()

ADD_DEFINITIONS(-DQT_NO_CAST_TO_ASCII)

SET(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)")
  # spatialite crashes on ppc - see bugs.debian.org/603986
  add_definitions( -fno-strict-aliasing )
endif()
if (CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
  message (STATUS "Debug output enabled")
  set(QGISDEBUG TRUE)
else()
  set(QGISDEBUG FALSE)
endif()
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
  # disable macros that offend std::numeric_limits<T>::min()/max()
  add_definitions(-DNOMINMAX)
endif()

# Prevent build when Qt api deprecated before this Qt version is used:
add_definitions(-DQT_DEPRECATED_WARNINGS)
# Unfortunately Qwt uses deprecated QString::null in headers, preventing this being raised above 5.8
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050800)

# For fast string concatenation
add_definitions(-DQT_USE_QSTRINGBUILDER)
if (ANDROID)
  string(REPLACE "<CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME>" "" CMAKE_CXX_CREATE_SHARED_MODULE "${CMAKE_CXX_CREATE_SHARED_MODULE}")
endif()
if("${CMAKE_SYSTEM_NAME}"  MATCHES "Linux")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-undefined")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-undefined")
endif() #"${CMAKE_SYSTEM_NAME}"  MATCHES "Linux")

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

#############################################################
# find QGIS
IF (USE_QGIS_BUILD_DIR)
    include_directories(
      ${QGIS_SRC_DIR}/src/core
      ${QGIS_SRC_DIR}/src/core/annotations
      ${QGIS_SRC_DIR}/src/core/auth
      ${QGIS_SRC_DIR}/src/core/composer
      ${QGIS_SRC_DIR}/src/core/fieldformatter
      ${QGIS_SRC_DIR}/src/core/geometry
      ${QGIS_SRC_DIR}/src/core/labeling
      ${QGIS_SRC_DIR}/src/core/layertree
      ${QGIS_SRC_DIR}/src/core/layout
      ${QGIS_SRC_DIR}/src/core/locator
      ${QGIS_SRC_DIR}/src/core/metadata
      ${QGIS_SRC_DIR}/src/core/providers/memory
      ${QGIS_SRC_DIR}/src/core/raster
      ${QGIS_SRC_DIR}/src/core/scalebar
      ${QGIS_SRC_DIR}/src/core/symbology
      ${QGIS_SRC_DIR}/src/core/textrenderer
      ${QGIS_SRC_DIR}/src/core/effects
      ${QGIS_SRC_DIR}/src/core/metadata
      ${QGIS_SRC_DIR}/src/core/expression
      ${QGIS_SRC_DIR}/external
      ${QGIS_SRC_DIR}/external/nlohmann
      ${QGIS_BUILD_DIR}/src/core
    )

    IF (APPLE OR IOS)
        # macos and ios
        SET(QGIS_CORE_LIB ${QGIS_BUILD_DIR}/output/lib/qgis_core.framework/qgis_core)
    ELSE (APPLE OR IOS)
        IF (ANDROID)
            MESSAGE(ERROR "android from build dir not supported")
        ELSE (ANDROID)
            IF (WIN32)
                MESSAGE(ERROR "windows from build dir not supported")
            ELSE (WIN32)
                # Linux
                SET(QGIS_CORE_LIB ${QGIS_BUILD_DIR}/output/lib/libqgis_core.so)
            ENDIF (WIN32)
        ENDIF (ANDROID)
    ENDIF (APPLE OR IOS)
ELSE(USE_QGIS_BUILD_DIR)
    IF (APPLE OR IOS)
        INCLUDE_DIRECTORIES(
            ${QGIS_INSTALL_PATH}/QGIS.app/Contents/Frameworks/qgis_core.framework/Headers
            ${QGIS_INSTALL_PATH}/QGIS.app/Contents/Frameworks/qgis_native.framework/Headers
        )
        SET(QGIS_CORE_LIB ${QGIS_INSTALL_PATH}/QGIS.app/Contents/Frameworks/qgis_core.framework/qgis_core)
    ELSE (APPLE OR IOS)
        IF (ANDROID)
            # android
            INCLUDE_DIRECTORIES( ${QGIS_INSTALL_PATH}/include/qgis)
            SET(QGIS_CORE_LIB ${QGIS_INSTALL_PATH}/lib/libqgis_core_${ANDROID_ABI}.so)
        ELSE (ANDROID)
            IF (WIN32)
                INCLUDE_DIRECTORIES( ${QGIS_INSTALL_PATH}/include)
                SET(QGIS_CORE_LIB ${QGIS_INSTALL_PATH}/bin/qgis_core.dll)
                SET(QGIS_CORE_IMPLIB ${QGIS_INSTALL_PATH}/lib/qgis_core.lib)
            ELSE (WIN32)
                # linux
                INCLUDE_DIRECTORIES( ${QGIS_INSTALL_PATH}/include/qgis)
                SET(QGIS_CORE_LIB ${QGIS_INSTALL_PATH}/lib/libqgis_core.so)
            ENDIF (WIN32)
        ENDIF (ANDROID)
    ENDIF (APPLE OR IOS)
ENDIF(USE_QGIS_BUILD_DIR)

#############################################################
# setup some vars required in qgsquick CMakeLists
SET(CPACK_PACKAGE_VERSION_MAJOR ${QGIS_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${QGIS_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${QGIS_VERSION_PATCH})
SET(COMPLETE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})
SET(RELEASE_NAME "Master")
MATH(EXPR QGIS_VERSION_INT "${CPACK_PACKAGE_VERSION_MAJOR}*10000+${CPACK_PACKAGE_VERSION_MINOR}*100+${CPACK_PACKAGE_VERSION_PATCH}")

IF (FORCE_STATIC_LIBS)
  SET (LIBRARY_TYPE STATIC)
ELSE()
  SET (LIBRARY_TYPE SHARED)
ENDIF()

# mimic what is done in building QGIS
set (HAVE_QUICK TRUE)
set (HAVE_GUI FALSE)
set (HAVE_POSTGRESQL TRUE)
set (HAVE_ORACLE FALSE)
set (SERVER_SKIP_ECW TRUE)
set (HAVE_SERVER_PYTHON_PLUGINS FALSE)
set (HAVE_OPENCL FALSE)
set (ENABLE_MODELTEST FALSE)
set (HAVE_3D FALSE)
set (HAVE_GEOREFERENCER FALSE)
set (HAVE_QT5SERIALPORT FALSE)
IF (FORCE_STATIC_LIBS)
  SET (LIBRARY_TYPE STATIC)
  set (HAVE_STATIC_PROVIDERS TRUE)
ELSE()
  SET (LIBRARY_TYPE SHARED)
  set (HAVE_STATIC_PROVIDERS FALSE)
ENDIF()
set (HAVE_EPT FALSE)
set (HAVE_PDAL FALSE)

SET (QGIS_LIB_SUBDIR  lib)
SET (QGIS_BIN_DIR     bin)
SET (QGIS_BIN_SUBDIR  bin)
SET (QGIS_CGIBIN_DIR  bin)
SET (QGIS_LIB_DIR     lib)
SET (QGIS_LIBEXEC_DIR .)
SET (QGIS_LIBEXEC_SUBDIR .)
SET (QGIS_DATA_DIR    .)
SET (QGIS_PLUGIN_DIR  plugins)
SET (QT_PLUGINS_DIR   ${QGIS_OUTPUT_DIRECTORY}/plugins)
SET (QGIS_INCLUDE_DIR include)
SET (QGIS_QML_DIR     qml)
SET (QGIS_FW_SUBDIR   frameworks)
SET (QGIS_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${QGIS_OUTPUT_DIRECTORY}/bin)
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${QGIS_OUTPUT_DIRECTORY}/lib)

CONFIGURE_FILE(qgsconfig.h.in ${CMAKE_BINARY_DIR}/qgsconfig.h)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

# include_directories(${Qt5Widgets_INCLUDE_DIRS})
ADD_LIBRARY(qgis_core ${LIBRARY_TYPE} IMPORTED GLOBAL)
SET_TARGET_PROPERTIES(qgis_core PROPERTIES
    IMPORTED_LOCATION ${QGIS_CORE_LIB}
    INTERFACE_LINK_LIBRARIES Qt5::Widgets
)
IF (WIN32)
    SET_TARGET_PROPERTIES(qgis_core PROPERTIES
        IMPORTED_IMPLIB ${QGIS_CORE_IMPLIB}
        IMPORTED_CONFIGURATIONS "RELEASE"
    )
ENDIF (WIN32)

SET(QMLPLUGINDUMP_FOUND FALSE)
#############################################################
# add original CMakeLists
ADD_SUBDIRECTORY(from_qgis)
