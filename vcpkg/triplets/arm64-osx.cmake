set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Darwin)
set(VCPKG_OSX_ARCHITECTURES arm64)
set(VCPKG_BUILD_TYPE release)

if(DEFINED ENV{DEPLOYMENT_TARGET})
  set(VCPKG_OSX_DEPLOYMENT_TARGET $ENV{DEPLOYMENT_TARGET})
else()
    message(FATAL_ERROR "DEPLOYMENT_TARGET not defined as env variable")
endif()
# See https://github.com/microsoft/vcpkg/issues/10038
set(VCPKG_C_FLAGS -mmacosx-version-min=${VCPKG_OSX_DEPLOYMENT_TARGET})
set(VCPKG_CXX_FLAGS -mmacosx-version-min=${VCPKG_OSX_DEPLOYMENT_TARGET})

