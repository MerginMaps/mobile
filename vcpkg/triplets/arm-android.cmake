set(VCPKG_TARGET_ARCHITECTURE arm)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Android)
set(VCPKG_BUILD_TYPE release)

set(VCPKG_MAKE_BUILD_TRIPLET "--host=arm64-darwin-androideabi")
# TODO linux
# set(VCPKG_MAKE_BUILD_TRIPLET "--host=armv7a-linux-androideabi")


set(VCPKG_CMAKE_CONFIGURE_OPTIONS -DANDROID_ABI=armeabi-v7a -DANDROID_ARM_NEON=ON)

if(DEFINED ENV{ANDROIDAPI})
  set(VCPKG_CMAKE_SYSTEM_VERSION $ENV{ANDROIDAPI})
else()
  message(FATAL_ERROR "ANDROIDAPI not defined as env variable")
endif()

set(VCPKG_CXX_FLAGS "-fstack-protector-strong")
set(VCPKG_C_FLAGS "-fstack-protector-strong")
set(VCPKG_LINKER_FLAGS "-lunwind -Wl,--exclude-libs=libunwind.a")
