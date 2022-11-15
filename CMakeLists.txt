cmake_minimum_required(VERSION 3.22)

set(MM_VERSION_MAJOR "2")
set(MM_VERSION_MINOR "0")
set(MM_VERSION_PATCH "0")

project(
  merginmaps
  VERSION ${MM_VERSION_MAJOR}.${MM_VERSION_MINOR}.${MM_VERSION_PATCH}
  DESCRIPTION "The easiest way to take your QGIS projects into the field"
  HOMEPAGE_URL "https://merginmaps.com"
  LANGUAGES CXX
)

set(CMAKE_COLOR_MAKEFILE ON)
# set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Exiv still uses std::auto_ptr
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_definitions(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
endif()
if (MSVC)
  add_compile_definitions(_HAS_AUTO_PTR_ETC=1)
endif()

set(USE_SERVER_API_KEY TRUE CACHE BOOL "Determines whether we should use server API key. Decrypt file with secrets on disk if you want to use it.")
set(QGIS_QUICK_DATA_PATH "INPUT" CACHE STRING "The internal variable pointing to the application storage folder") 
set(INPUT_SDK_PATH "" CACHE PATH "Path to the Mergin Maps Input SDK on host machine" )
set(ENABLE_TESTS TRUE CACHE BOOL "Whether to build tests")

find_package(Qt6 COMPONENTS Quick Qml Xml Concurrent Positioning Sensors QuickControls2 Network Svg Sql OpenGl Core Core5Compat REQUIRED)
find_package(Geodiff REQUIRED)
find_package(QGIS REQUIRED)
find_package(Proj REQUIRED)
find_package(ZXing REQUIRED)

if(NOT MSVC) 
  # Hundreds of warnings from QGIS core:
  # warning: 'Type' is deprecated: Use QMetaType::Type instead
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif()

if (APPLE)
    add_compile_definitions(DESKTOP_OS)
    add_compile_definitions(PURCHASING) # PURCHASING stuff (only testing)
    add_compile_definitions(HAVE_BLUETOOTH)
    add_compile_definitions(HAVE_WIDGETS)
else()
    add_compile_definitions(MOBILE_OS)
endif()


if (ENABLE_TESTS)
  enable_testing()
  find_package(Qt6 COMPONENTS Test REQUIRED)
  add_compile_definitions(INPUT_TEST)
endif()

add_subdirectory(core)
add_subdirectory(app)
