PROJEct(qgismobile)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

SET(CMAKE_COLOR_MAKEFILE ON)

FIND_PACKAGE(Qt5Core QUIET)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Qml REQUIRED)
FIND_PACKAGE(Qt5Quick REQUIRED)
FIND_PACKAGE(Qt5Positioning REQUIRED)
FIND_PACKAGE(Qt5Xml REQUIRED)     # QGIS dep
FIND_PACKAGE(Qt5Widgets REQUIRED) # QGIS dep
FIND_PACKAGE(Qt5Concurrent REQUIRED) # QGIS dep
MESSAGE(STATUS "Found Qt version: ${Qt5Core_VERSION_STRING}")

# QGIS
set(QGIS_PATH /home/martin/inst/qgis-core-qt5 CACHE PATH "Path to QGIS installation")
set(QGIS_INCLUDE_DIR ${QGIS_PATH}/include/qgis)
set(QGIS_LIBRARY ${QGIS_PATH}/lib/libqgis_core.so)
SET(EXTRA_INCLUDE_DIR ${QGIS_PATH}/include) # some other dependent libs (e.g. geos, gdal)


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

set(plugin_srcs
  plugin/plugin.cpp
  plugin/plugin.h
  plugin/mapengine.cpp
  plugin/mapengine.h
  plugin/maplayer.cpp
  plugin/maplayer.h
  plugin/project.cpp
  plugin/project.h
  plugin/positionengine.cpp
  plugin/positionengine.h
)

set(test_srcs
  test/main.cpp
  test/main.qml
  test/IdentifyBar.qml
  test/IdentifyContentItem.qml
)

# QML plugin
configure_file(plugin/qmldir output/qgis/qmldir COPYONLY)
configure_file(plugin/MapCanvas.qml output/qgis/MapCanvas.qml COPYONLY)
configure_file(plugin/ScaleBar.qml output/qgis/ScaleBar.qml COPYONLY)
configure_file(plugin/VertexMarker.qml output/qgis/VertexMarker.qml COPYONLY)
add_library(qgismobile MODULE ${plugin_srcs})
target_link_libraries(qgismobile Qt5::Quick Qt5::Qml ${QGIS_LIBRARY} Qt5::Xml Qt5::Widgets Qt5::Concurrent Qt5::Positioning)
set_target_properties(qgismobile PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/qgis)
include_directories(${QGIS_INCLUDE_DIR})
include_directories(${EXTRA_INCLUDE_DIR})
add_definitions(-DCORE_EXPORT=)
if (NOT ANDROID)
add_definitions(-DQGIS_PREFIX_PATH="${QGIS_PATH}")
endif ()

# QML plugin installation
SET(QM_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/qml/qgis)
INSTALL(TARGETS qgismobile
  RUNTIME DESTINATION ${QM_INSTALL_DIR}
  LIBRARY DESTINATION ${QM_INSTALL_DIR}
)
INSTALL(FILES plugin/qmldir plugin/MapCanvas.qml plugin/ScaleBar.qml plugin/VertexMarker.qml
        DESTINATION ${QM_INSTALL_DIR})


qt5_add_resources(RESOURCE_ADDED test/test.qrc)

# test executable
add_executable(qgismobiletest ${RESOURCE_ADDED} ${test_srcs})
target_link_libraries(qgismobiletest Qt5::Gui Qt5::Qml Qt5::Quick ${qgis_library} Qt5::Xml Qt5::Widgets)
set_target_properties(qgismobiletest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
