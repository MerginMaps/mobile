set(VCPKG_TARGET_ARCHITECTURE arm)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Android)
set(VCPKG_BUILD_TYPE release)
set(VCPKG_MAKE_BUILD_TRIPLET "--host=armv7a-linux-androideabi")
set(VCPKG_CMAKE_CONFIGURE_OPTIONS -DANDROID_ABI=armeabi-v7a -DANDROID_ARM_NEON=ON)

if(DEFINED ENV{ANDROIDAPI})
  set(VCPKG_CMAKE_SYSTEM_VERSION $ENV{ANDROIDAPI})
endif()

set(VCPKG_CXX_FLAGS "-fstack-protector-strong")
set(VCPKG_C_FLAGS "-fstack-protector-strong")

set(VCPKG_LINKER_FLAGS "-lunwind -Wl,--exclude-libs=libunwind.a")

set(VCPKG_ENV_PASSTHROUGH Qt6_DIR) # needed by port/qt6/vcpkg-cmake-wrapper.cmake
set(VCPKG_ENV_PASSTHROUGH QT_HOST_PATH) # needed by port/qt6/vcpkg-cmake-wrapper.cmake
set(QT_HOST_PATH $ENV{QT_HOST_PATH} CACHE PATH "needed by QtPublicDependencyHelpers.cmake" FORCE) 