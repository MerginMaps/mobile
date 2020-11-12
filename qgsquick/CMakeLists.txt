# QgsQuick
# Copyright (C) 2020 Peter Petrik (zilolv at gmail dot com)

CMAKE_MINIMUM_REQUIRED(VERSION 3.9)
PROJECT(QgsQuick)

SET (CMAKE_CXX_STANDARD 11)
# set path to additional CMake modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

#############################################################
# global vars
SET (QGIS_SRC_DIR "" CACHE PATH "QGIS source directory, for find packages and for includes if USE_QGIS_BUILD_DIR=TRUE")
SET (USE_QGIS_BUILD_DIR FALSE CACHE BOOL "Use QGIS from build directory")
SET (QGIS_INSTALL_PATH "" CACHE PATH "QGIS install directory, only used when USE_QGIS_BUILD_DIR=FALSE")
SET (QGIS_BUILD_DIR "" CACHE PATH "QGIS build directory, only used when USE_QGIS_BUILD_DIR=TRUE")
set (FORCE_STATIC_LIBS ${DEFAULT_FORCE_STATIC_LIBS} CACHE BOOL "Determines whether libraries should be static only")

if (APPLE)
  set (QGIS_MAC_DEPS_DIR "" CACHE PATH "Path to QGIS Mac custom dependencies directory")
  # Setup LIB_DIR and CMAKE_PREFIX_PATH to help CMake's
  # find_packages to look for these libraries instead of system libraries
  if ( QGIS_MAC_DEPS_DIR )
    set(ENV{LIB_DIR} ${QGIS_MAC_DEPS_DIR})
    list(APPEND CMAKE_PREFIX_PATH ${QGIS_MAC_DEPS_DIR})
  endif()
endif()

# set path to additional CMake modules
set(CMAKE_MODULE_PATH ${QGIS_SRC_DIR}/cmake ${CMAKE_MODULE_PATH})

#############################################################
# Find Packages
SET(CMAKE_SOURCE_DIR_MINE ${CMAKE_SOURCE_DIR})
SET(CMAKE_SOURCE_DIR ${QGIS_SRC_DIR})
  find_package(Proj)
  find_package(GEOS)
  find_package(GDAL)
  find_package(Expat REQUIRED)
  find_package(Spatialindex REQUIRED)
  find_package(LibZip REQUIRED)
  find_package(Sqlite3)
  find_package(SpatiaLite REQUIRED)
  find_package(QtKeychain REQUIRED)
  find_package(QCA REQUIRED)
SET(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR_MINE})

set(QT_MIN_VERSION 5.9.0)
find_package(Qt5 COMPONENTS Core Gui Widgets Network Xml Svg Concurrent Test UiTools Sql REQUIRED)
if (NOT IOS)
  find_package(Qt5 COMPONENTS PrintSupport REQUIRED)
else()
  add_definitions(-DQT_NO_PRINTER)
endif()
find_package(Qt5 COMPONENTS Positioning)
if (APPLE)
  find_package(Qt5 COMPONENTS MacExtras REQUIRED)
endif()

find_package(Qt5 COMPONENTS Qml Quick REQUIRED)
if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    find_package(Qt5 COMPONENTS AndroidExtras)
else()
    find_package(QtQmlTools)
endif()

#############################################################
# warnings https://stackoverflow.com/a/3818084/2838364
IF(MSVC)
  # Force to always compile with W4
  IF(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    STRING(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  ELSE()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  ENDIF()
ELSEIF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR APPLE )
  # Update if necessary
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
ENDIF(MSVC)


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
    SET(QGIS_CORE ${QGIS_BUILD_DIR}/output/QGIS.app/Contents/Frameworks/qgis_core.framework/qgis_core)
ELSE(USE_QGIS_BUILD_DIR)
    include_directories( ${QGIS_INSTALL_PATH}/QGIS.app/Contents/MacOS/include/ )
    SET(QGIS_CORE ${QGIS_INSTALL_PATH}/QGIS.app/Contents/Frameworks/qgis_core.framework/qgis_core)
ENDIF(USE_QGIS_BUILD_DIR)

SET(qgis_core $QGIS_CORE)

#############################################################
# setup some vars required in qgsquick CMakeLists
if (FORCE_STATIC_LIBS)
  set (LIBRARY_TYPE STATIC)
else()
  set (LIBRARY_TYPE SHARED)
endif()

set (HAVE_QUICK TRUE)

set (QGIS_LIB_SUBDIR  lib)
set (QGIS_BIN_DIR     bin)
set (QGIS_CGIBIN_DIR  bin)
set (QGIS_LIB_DIR     lib)
set (QGIS_LIBEXEC_DIR .)
set (QGIS_DATA_DIR    .)
set (QGIS_PLUGIN_DIR  plugins)
set (QT_PLUGINS_DIR   ${QGIS_OUTPUT_DIRECTORY}/plugins)
set (QGIS_INCLUDE_DIR include)
set (QGIS_QML_DIR     qml)
set (QGIS_FW_SUBDIR   frameworks)
set (QGIS_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${QGIS_OUTPUT_DIRECTORY}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${QGIS_OUTPUT_DIRECTORY}/lib)

#############################################################
# add original CMakeLists
ADD_SUBDIRECTORY(from_qgis)
