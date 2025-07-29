set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Android)
set(VCPKG_BUILD_TYPE release)

# Note to myself and others: this is correct even on macOS host
set(VCPKG_MAKE_BUILD_TRIPLET "--host=aarch64-linux-android")

set(VCPKG_CMAKE_CONFIGURE_OPTIONS -DANDROID_ABI=arm64-v8a)

set(ANDROIDAPI "28") # when changed change CMakeLists.cmake
set(ANDROID_PLATFORM "android-${ANDROIDAPI}")
set(ANDROID_NDK_PLATFORM "android-${ANDROIDAPI}")
set(VCPKG_CMAKE_SYSTEM_VERSION "${ANDROIDAPI}")
set(ANDROID_STL "c++_shared")

set(VCPKG_CXX_FLAGS "-fstack-protector-strong")
set(VCPKG_C_FLAGS "-fstack-protector-strong")