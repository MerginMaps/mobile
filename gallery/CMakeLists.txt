cmake_minimum_required(VERSION 3.16)

project(
  MerginMapsGallery
  VERSION 0.1
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT6_VERSION
    6.6.0
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
             Multimedia
  REQUIRED
)

qt_standard_project_setup()
qt_policy(SET QTP0001 NEW)

set(GALLERY_HDRS
    helper.h
    qrcodedecoder.h
    inpututils.h
    scalebarkit.h
    positionkit.h
    formfeaturesmodel.h
    enums.h
    ../app/notificationmodel.h
    ../app/inpututils_p.h
    ../app/mmstyle.h
    ../core/merginerrortypes.h
)

set(GALLERY_SRCS
    helper.cpp
    ../app/notificationmodel.cpp
    ../app/inpututils_p.cpp
    main.cpp
)

if (IOS OR ANDROID)
  add_compile_definitions(MOBILE_OS)
else ()
  add_compile_definitions(DESKTOP_OS)

  set(GALLERY_HDRS ${GALLERY_HDRS} hotreload.h)
  set(GALLERY_SRCS ${GALLERY_SRCS} hotreload.cpp)
endif ()

add_definitions(-DGALLERY_APP)

qt_add_executable(
  MerginMapsGallery
  ${GALLERY_SRCS}
  ${GALLERY_HDRS}
  qml.qrc
  ../app/fonts/fonts.qrc
  ../app/icons/icons.qrc
  ../app/images/images.qrc
)

qt_add_qml_module(
  MerginMapsGallery
  URI gallery
  VERSION 1.0
  QML_FILES qml/Main.qml
            NO_GENERATE_QMLTYPES
            RESOURCES
            qml.qrc
)

qt_add_resources(../app/fonts/fonts.qrc)

set_target_properties(
  MerginMapsGallery
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER uk.co.lutraconsulting.merginmaps.gallery
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             MACOSX_BUNDLE TRUE
             WIN32_EXECUTABLE TRUE
)

target_link_libraries(MerginMapsGallery PRIVATE Qt6::Quick Qt6::Multimedia)
target_include_directories(MerginMapsGallery PRIVATE ${CMAKE_SOURCE_DIR} ../app ../core)
target_compile_definitions(
  MerginMapsGallery
  PRIVATE GALLERY_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

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
