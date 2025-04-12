set(VCPKG_TARGET_ARCHITECTURE arm)
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
set(VCPKG_MAKE_BUILD_TRIPLET "--host=armv7a-linux-androideabi")

set(VCPKG_CMAKE_CONFIGURE_OPTIONS -DANDROID_ABI=armeabi-v7a -DANDROID_ARM_NEON=ON)

set(ANDROIDAPI "28") # when changed change CMakeLists.txt
set(ANDROID_PLATFORM "android-${ANDROIDAPI}")
set(ANDROID_NDK_PLATFORM "android-${ANDROIDAPI}")
set(ANDROID_STL "c++_shared")

if (NOT DEFINED ENV{ANDROID_SDK_ROOT})
  message(FATAL_ERROR "Set required environment variable ANDROID_SDK_ROOT.")
endif ()
set(ANDROID_SDK_ROOT $ENV{ANDROID_SDK_ROOT})

if(DEFINED ENV{ANDROIDAPI})
  set(VCPKG_CMAKE_SYSTEM_VERSION $ENV{ANDROIDAPI})
endif()

set(VCPKG_CXX_FLAGS "-fstack-protector-strong")
set(VCPKG_C_FLAGS "-fstack-protector-strong")

set(VCPKG_LINKER_FLAGS "-lunwind -Wl,--exclude-libs=libunwind.a")

set(VCPKG_ENV_PASSTHROUGH Qt6_DIR) # needed by port/qt6/vcpkg-cmake-wrapper.cmake
set(VCPKG_ENV_PASSTHROUGH QT_HOST_PATH) # needed by port/qt6/vcpkg-cmake-wrapper.cmake
set(QT_HOST_PATH $ENV{QT_HOST_PATH} CACHE PATH "needed by QtPublicDependencyHelpers.cmake" FORCE)