set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Android)
set(VCPKG_BUILD_TYPE release)


set(VCPKG_MAKE_BUILD_TRIPLET "--host=arm64-darwin-android")
# TODO linux
# set(VCPKG_MAKE_BUILD_TRIPLET "--host=aarch64-linux-android")

set(VCPKG_CMAKE_CONFIGURE_OPTIONS -DANDROID_ABI=arm64-v8a)

set(VCPKG_CXX_FLAGS "-fstack-protector-strong")
set(VCPKG_C_FLAGS "-fstack-protector-strong")

if(DEFINED ENV{ANDROIDAPI})
  set(VCPKG_CMAKE_SYSTEM_VERSION $ENV{ANDROIDAPI})
else()
    message(FATAL_ERROR "$ENV{ANDROIDAPI} not defined as env variable")
endif()

