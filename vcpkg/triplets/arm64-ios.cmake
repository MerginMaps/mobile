set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME iOS)
set(VCPKG_OSX_ARCHITECTURES arm64)

if(DEFINED ENV{DEPLOYMENT_TARGET})
  set(VCPKG_OSX_DEPLOYMENT_TARGET $ENV{DEPLOYMENT_TARGET})
endif()

set(VCPKG_BUILD_TYPE release)
set(VCPKG_CXX_FLAGS "-fvisibility=hidden")
set(VCPKG_C_FLAGS "-fvisibility=hidden")

set(VCPKG_ENV_PASSTHROUGH Qt6_DIR) # needed by port/qt6/vcpkg-cmake-wrapper.cmake
set(VCPKG_ENV_PASSTHROUGH QT_HOST_PATH) # needed by port/qt6/vcpkg-cmake-wrapper.cmake
set(QT_HOST_PATH $ENV{QT_HOST_PATH} CACHE PATH "needed by QtPublicDependencyHelpers.cmake" FORCE)