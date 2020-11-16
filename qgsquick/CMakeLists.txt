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
SET (FORCE_STATIC_LIBS ${DEFAULT_FORCE_STATIC_LIBS} CACHE BOOL "Determines whether libraries should be static only")
SET (QGIS_VERSION_MAJOR "" CACHE STRING "QGIS VERSION STRING")
SET (QGIS_VERSION_MINOR "" CACHE STRING "QGIS VERSION STRING")
SET (QGIS_VERSION_PATCH "" CACHE STRING "QGIS VERSION STRING")
SET (ANDROID_TARGET_ARCH "" CACHE STRING "ANDROID_TARGET_ARCH")

IF (APPLE)
  SET (QGIS_MAC_DEPS_DIR "" CACHE PATH "Path to QGIS Mac custom dependencies directory")
  IF ( QGIS_MAC_DEPS_DIR )
    SET(ENV{LIB_DIR} ${QGIS_MAC_DEPS_DIR})
    LIST(APPEND CMAKE_PREFIX_PATH ${QGIS_MAC_DEPS_DIR})
  ENDIF()
ENDIF()

SET(CMAKE_MODULE_PATH ${QGIS_CMAKE_PATH} ${CMAKE_MODULE_PATH})

#############################################################
# Find Packages
INCLUDE(GenerateExportHeader)
SET(CMAKE_SOURCE_DIR_MINE ${CMAKE_SOURCE_DIR})
SET(CMAKE_SOURCE_DIR ${QGIS_CMAKE_PATH}/..)
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
                SET(QGIS_CORE_LIB ${QGIS_INSTALL_PATH}/output/lib/libqgis_core.so)
            ENDIF (WIN32)

        ENDIF (ANDROID)

        SET(QGIS_CORE_LIB ${QGIS_BUILD_DIR}/output/lib)
    ENDIF (APPLE OR IOS)
ELSE(USE_QGIS_BUILD_DIR)
    IF (APPLE OR IOS)
        INCLUDE_DIRECTORIES(
            ${QGIS_INSTALL_PATH}/QGIS.app/Contents/Frameworks/qgis_core.framework/Versions/Current/Headers/
            ${QGIS_INSTALL_PATH}/QGIS.app/Contents/Frameworks/qgis_native.framework/Headers
        )
        SET(QGIS_CORE_LIB ${QGIS_INSTALL_PATH}/QGIS.app/Contents/Frameworks/qgis_core.framework/qgis_core)
    ELSE (APPLE OR IOS)
        IF (ANDROID)
            # android
            INCLUDE_DIRECTORIES( ${QGIS_INSTALL_PATH}/include/qgis)
            SET(QGIS_CORE_LIB ${QGIS_INSTALL_PATH}/lib/libqgis_core_${ANDROID_TARGET_ARCH}.so)
        ELSE (ANDROID)
            IF (WIN32)
                INCLUDE_DIRECTORIES( ${QGIS_INSTALL_PATH}/include)
                SET(QGIS_CORE_LIB ${QGIS_INSTALL_PATH}/lib/libqgis_core.lib)
            ELSE (WIN32)
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

set (HAVE_QUICK TRUE)

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

SET(QMLPLUGINDUMP_FOUND FALSE)
#############################################################
# add original CMakeLists
ADD_SUBDIRECTORY(from_qgis)
