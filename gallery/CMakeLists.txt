cmake_minimum_required(VERSION 3.16)

project(
  MerginMapsGallery
  VERSION 0.1
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT6_VERSION
    6.5.2
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

qt_standard_project_setup()
qt_policy(SET QTP0001 NEW)

set(GALLERY_HDRS helper.h notificationmodel.h)

set(GALLERY_SRCS helper.cpp notificationmodel.cpp)

if (IOS OR ANDROID)
  add_compile_definitions(MOBILE_OS)
else ()
  add_compile_definitions(DESKTOP_OS)

  set(GALLERY_HDRS ${GALLERY_HDRS} hotreload.h)
  set(GALLERY_SRCS ${GALLERY_SRCS} hotreload.cpp)

  # todo: cp -r app/qmlV2 gallery/app/

endif ()

qt_add_executable(
  MerginMapsGallery
  ${GALLERY_SRCS}
  ${GALLERY_HDRS}
  main.cpp
  qml.qrc
  fonts.qrc
  ../app/icons/icons.qrc
)

qt_add_qml_module(
  MerginMapsGallery
  URI gallery
  VERSION 1.0
  QML_FILES qml/Main.qml
            NO_GENERATE_QMLTYPES
            RESOURCES
            mobile.qrc
)

qt_add_resources(fonts.qrc)

set_target_properties(
  MerginMapsGallery
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             MACOSX_BUNDLE TRUE
             WIN32_EXECUTABLE TRUE
)

target_link_libraries(MerginMapsGallery PRIVATE Qt6::Quick)

install(
  TARGETS MerginMapsGallery
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_qml_app_script(
  TARGET
  MerginMapsGallery
  OUTPUT_SCRIPT
  deploy_script
  MACOS_BUNDLE_POST_BUILD
  NO_UNSUPPORTED_PLATFORM_ERROR
  DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
)
install(SCRIPT ${deploy_script})
