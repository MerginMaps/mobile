set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)

# Workaround for bad patchelf interaction with binutils 2.43.50 necessary for Fedora < 42
# https://github.com/LadybirdBrowser/ladybird/issues/2149
# https://github.com/microsoft/vcpkg/issues/41576
# https://github.com/NixOS/patchelf/issues/568
# https://bugzilla.redhat.com/show_bug.cgi?id=2319341
if (NOT LAGOM_USE_LINKER)
  set(ENV{LDFLAGS} -Wl,-z,noseparate-code)
endif ()

include("${CMAKE_CURRENT_LIST_DIR}/static-ports.cmake")
if (PORT IN_LIST STATIC_PORTS)
  set(VCPKG_LIBRARY_LINKAGE static)
else ()
  set(VCPKG_LIBRARY_LINKAGE dynamic)
endif ()

set(VCPKG_CMAKE_SYSTEM_NAME Linux)
set(VCPKG_BUILD_TYPE release)

set(VCPKG_FIXUP_ELF_RPATH ON)
