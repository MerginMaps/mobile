PROJEct(qgismobile)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

SET(CMAKE_COLOR_MAKEFILE ON)

FIND_PACKAGE(Qt5Core QUIET)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Qml REQUIRED)
FIND_PACKAGE(Qt5Quick REQUIRED)
MESSAGE(STATUS "Found Qt version: ${Qt5Core_VERSION_STRING}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(plugin_srcs
  plugin/plugin.cpp
  plugin/plugin.h
  plugin/mapcanvas.cpp
  plugin/mapcanvas.h
)

set(test_srcs
  test/main.cpp
  test/main.qml
)

# QML plugin
configure_file(plugin/qmldir output/qgis/qmldir COPYONLY)
configure_file(plugin/TestItem.qml output/qgis/TestItem.qml COPYONLY)
add_library(qgismobile MODULE ${plugin_srcs})
target_link_libraries(qgismobile Qt5::Quick Qt5::Qml)
set_target_properties(qgismobile PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/qgis)


qt5_add_resources(RESOURCE_ADDED test/test.qrc)

# test executable
add_executable(qgismobiletest ${RESOURCE_ADDED} ${test_srcs})
target_link_libraries(qgismobiletest Qt5::Gui Qt5::Qml)
set_target_properties(qgismobiletest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
