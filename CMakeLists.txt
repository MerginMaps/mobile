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
set(CMAKE_AUTORCC ON)
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

set(USE_MM_SERVER_API_KEY TRUE CACHE BOOL "Determines whether we should use server API key. Decrypt file with secrets on disk if you want to use it.")
set(QGIS_QUICK_DATA_PATH "INPUT" CACHE STRING "The internal variable pointing to the application storage folder") 
set(MM_SDK_PATH "" CACHE PATH "Path to the Mergin Maps Input SDK on host machine" )

find_package(Qt6 COMPONENTS Quick Qml Xml Concurrent Positioning Sensors QuickControls2 Network Svg Sql OpenGl Core Core5Compat REQUIRED)
find_package(MerginMapsSDK)

add_subdirectory(core)
add_subdirectory(app)
