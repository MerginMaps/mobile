set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME iOS)
set(VCPKG_OSX_ARCHITECTURES arm64)

set(VCPKG_OSX_DEPLOYMENT_TARGET 16.0)
set(VCPKG_MAKE_BUILD_TRIPLET "--host=aarch64-apple-ios${VCPKG_OSX_DEPLOYMENT_TARGET}")

set(VCPKG_BUILD_TYPE release)
set(VCPKG_CXX_FLAGS "-fvisibility=hidden")
set(VCPKG_C_FLAGS "-fvisibility=hidden")

include("${CMAKE_CURRENT_LIST_DIR}/qgis-build-type.cmake")
