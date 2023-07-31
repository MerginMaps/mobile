cmake_minimum_required(VERSION 3.16)

project(
  gallery
  VERSION 0.1
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT6_VERSION
    ${QT_VERSION_DEFAULT}
    CACHE STRING "QT6 version to use"
)
set(CMAKE_AUTORCC ON)

find_package(
  Qt6 ${QT6_VERSION}
  COMPONENTS Quick
             Qml
             Concurrent
             Positioning
             QuickControls2
             Svg
             Core
             Core5Compat
  REQUIRED
)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(
  appGallery
  main.cpp
  qml.qrc
  fonts.qrc
  hotreload.cpp
  hotreload.h
  helper.cpp
  helper.h
)

qt_add_qml_module(
  appGallery
  URI gallery
  VERSION 1.0
  QML_FILES qml/Main.qml
)

set_target_properties(
  appGallery
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             MACOSX_BUNDLE TRUE
             WIN32_EXECUTABLE TRUE
)

target_link_libraries(appGallery PRIVATE Qt6::Quick)

install(
  TARGETS appGallery
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_add_resources(fonts.qrc)
