PROJECT(qgis_quick_components)

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
  plugin/mapimage.cpp
  plugin/mapimage.h
  plugin/maplayer.cpp
  plugin/maplayer.h
  plugin/mapview.cpp
  plugin/mapview.h
  plugin/project.cpp
  plugin/project.h
  plugin/positionengine.cpp
  plugin/positionengine.h
)

set(plugin_qmls
  plugin/qmldir
  plugin/MapCanvas.qml
  plugin/ScaleBar.qml
  plugin/VertexMarker.qml
  plugin/VertexMarkerBase.qml
  plugin/PositionMarker.qml
  plugin/PositionAccuracy.qml
)

set(test_srcs
  test/main.cpp
  test/main.qml
  test/IdentifyBar.qml
  test/IdentifyContentItem.qml
)

# QML plugin
foreach (qml_file ${plugin_qmls})
  get_filename_component(qml_filename ${qml_file} NAME)
  configure_file(${qml_file} output/qgis/${qml_filename} COPYONLY)
endforeach(qml_file)
add_library(qgis-quick-components MODULE ${plugin_srcs})
target_link_libraries(qgis-quick-components Qt5::Quick Qt5::Qml ${QGIS_LIBRARY} Qt5::Xml Qt5::Widgets Qt5::Concurrent Qt5::Positioning)
set_target_properties(qgis-quick-components PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/qgis)
include_directories(${QGIS_INCLUDE_DIR})
include_directories(${EXTRA_INCLUDE_DIR})
add_definitions(-DCORE_EXPORT=)
if (NOT ANDROID)
add_definitions(-DQGIS_PREFIX_PATH="${QGIS_PATH}")
endif ()

# QML plugin installation
SET(QM_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/qml/qgis)
INSTALL(TARGETS qgis-quick-components
  RUNTIME DESTINATION ${QM_INSTALL_DIR}
  LIBRARY DESTINATION ${QM_INSTALL_DIR}
)
INSTALL(FILES ${plugin_qmls}
        DESTINATION ${QM_INSTALL_DIR})


qt5_add_resources(RESOURCE_ADDED test/test.qrc)

# test executable
add_executable(qgis-quick-components-test ${RESOURCE_ADDED} ${test_srcs})
target_link_libraries(qgis-quick-components-test Qt5::Gui Qt5::Qml Qt5::Quick Qt5::Xml Qt5::Widgets)
target_link_libraries(qgis-quick-components-test ${QGIS_LIBRARY})
set_target_properties(qgis-quick-components-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

INSTALL(TARGETS qgis-quick-components-test DESTINATION "bin")
INSTALL(TARGETS qgis-quick-components DESTINATION "lib")
