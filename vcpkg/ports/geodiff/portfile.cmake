# Be sure to update both of these versions together.
set(GEODIFF_VERSION 2.2.0)
set(GEODIFF_HASH
    b6ddd579dc446e6c22b520f4f94c49dbef42bba9deca55848acbd2b4993ae1236304cd1f07f4f7de91267901495bfb60d76668e8152625125778697cb57faabf
)

vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  MerginMaps/geodiff
  REF
  ${GEODIFF_VERSION}
  SHA512
  ${GEODIFF_HASH}
  HEAD_REF
  master
)

if (VCPKG_LIBRARY_LINKAGE STREQUAL "static")
  list(APPEND GEODIFF_LINK_OPTIONS -DBUILD_STATIC=ON)
  list(APPEND GEODIFF_LINK_OPTIONS -DBUILD_SHARED=OFF)
else ()
  list(APPEND GEODIFF_LINK_OPTIONS -DBUILD_STATIC=OFF)
  list(APPEND GEODIFF_LINK_OPTIONS -DBUILD_SHARED=ON)
endif ()

set(GEODIFF_OPTIONS
  -DENABLE_TESTS=OFF
  -DBUILD_TOOLS=OFF
  -DWITH_POSTGRESQL=OFF
  ${GEODIFF_LINK_OPTIONS}
)

if(VCPKG_TARGET_IS_OSX OR VCPKG_TARGET_IS_IOS)
  list(APPEND GEODIFF_OPTIONS "-DCMAKE_CXX_FLAGS=-Wno-error=deprecated-literal-operator")
endif()

vcpkg_configure_cmake(
  SOURCE_PATH ${SOURCE_PATH}/geodiff
  OPTIONS ${GEODIFF_OPTIONS}
)

vcpkg_install_cmake()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

file(WRITE ${CURRENT_PACKAGES_DIR}/share/${PORT}/copyright "Geodiff is MIT licensed\n")
configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/vcpkg-cmake-wrapper.cmake
  ${CURRENT_PACKAGES_DIR}/share/${PORT}/vcpkg-cmake-wrapper.cmake @ONLY
)
file(INSTALL ${CMAKE_CURRENT_LIST_DIR}/usage
     DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT}
)
