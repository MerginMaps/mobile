PROJEct(qgismobile)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

SET(CMAKE_COLOR_MAKEFILE ON)

FIND_PACKAGE(Qt5Core QUIET)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Qml REQUIRED)
FIND_PACKAGE(Qt5Quick REQUIRED)
FIND_PACKAGE(Qt5Xml REQUIRED)     # QGIS dep
FIND_PACKAGE(Qt5Widgets REQUIRED) # QGIS dep
FIND_PACKAGE(Qt5Concurrent REQUIRED) # QGIS dep
MESSAGE(STATUS "Found Qt version: ${Qt5Core_VERSION_STRING}")

# QGIS
set(qgis_path /home/martin/inst/qgis-core-qt5)
set(qgis_include_dir ${qgis_path}/include/qgis)
set(qgis_library ${qgis_path}/lib/libqgis_core.so)


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

SET(CMAKE_CXX_FLAGS "-std=c++0x")

set(plugin_srcs
  plugin/plugin.cpp
  plugin/plugin.h
  plugin/mapengine.cpp
  plugin/mapengine.h
  plugin/maplayer.cpp
  plugin/maplayer.h
)

set(test_srcs
  test/main.cpp
  test/main.qml
)

# QML plugin
configure_file(plugin/qmldir output/qgis/qmldir COPYONLY)
configure_file(plugin/MapCanvas.qml output/qgis/MapCanvas.qml COPYONLY)
add_library(qgismobile MODULE ${plugin_srcs})
target_link_libraries(qgismobile Qt5::Quick Qt5::Qml ${qgis_library} Qt5::Xml Qt5::Widgets Qt5::Concurrent)
set_target_properties(qgismobile PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/qgis)
include_directories(${qgis_include_dir})
add_definitions(-DCORE_EXPORT=)
add_definitions(-DQGIS_PREFIX_PATH="${qgis_path}")


qt5_add_resources(RESOURCE_ADDED test/test.qrc)

# test executable
add_executable(qgismobiletest ${RESOURCE_ADDED} ${test_srcs})
target_link_libraries(qgismobiletest Qt5::Gui Qt5::Qml Qt5::Quick ${qgis_library} Qt5::Xml Qt5::Widgets)
set_target_properties(qgismobiletest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
