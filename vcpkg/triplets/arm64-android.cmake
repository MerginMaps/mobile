set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
include("${CMAKE_CURRENT_LIST_DIR}/static-ports.cmake")
if (PORT IN_LIST STATIC_PORTS)
    set(VCPKG_LIBRARY_LINKAGE static)
else()
    set(VCPKG_LIBRARY_LINKAGE dynamic)
endif()
set(VCPKG_CMAKE_SYSTEM_NAME Android)
set(VCPKG_BUILD_TYPE release)

# Note to myself and others: this is correct even on macOS host
set(VCPKG_MAKE_BUILD_TRIPLET "--host=aarch64-linux-android")

set(VCPKG_CMAKE_CONFIGURE_OPTIONS -DANDROID_ABI=arm64-v8a)

set(ANDROIDAPI "28") # when changed change CMakeLists.txt
set(ANDROID_PLATFORM "android-${ANDROIDAPI}")
set(ANDROID_NDK_PLATFORM "android-${ANDROIDAPI}")
set(VCPKG_CMAKE_SYSTEM_VERSION "${ANDROIDAPI}")
set(ANDROID_STL "c++_shared")

if (NOT DEFINED ENV{ANDROID_SDK_ROOT})
  message(FATAL_ERROR "Set required environment variable ANDROID_SDK_ROOT.")
endif ()
set(ANDROID_SDK_ROOT $ENV{ANDROID_SDK_ROOT})

set(VCPKG_CXX_FLAGS "-fstack-protector-strong")
set(VCPKG_C_FLAGS "-fstack-protector-strong")