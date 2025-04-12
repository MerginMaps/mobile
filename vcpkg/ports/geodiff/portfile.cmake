# Be sure to update both of these versions together.
set(GEODIFF_VERSION 2.0.2)
set(GEODIFF_HASH 290f10ec1a0d419d13996e915d02b7d226f23359c2745a72a1a286d439bed3afdf536d5bb8771c3d4fb3df2d56ac5c0d4c7f178ee31493f4bf02d597c8b9a878)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO MerginMaps/geodiff
    REF ${GEODIFF_VERSION}
    SHA512 ${GEODIFF_HASH}
    HEAD_REF master
)

if(VCPKG_LIBRARY_LINKAGE STREQUAL "static")
    list(APPEND GEODIFF_LINK_OPTIONS -DBUILD_STATIC=ON)
    list(APPEND GEODIFF_LINK_OPTIONS -DBUILD_SHARED=OFF)
else()
    list(APPEND GEODIFF_LINK_OPTIONS -DBUILD_STATIC=OFF)
    list(APPEND GEODIFF_LINK_OPTIONS -DBUILD_SHARED=ON)
endif()


vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}/geodiff
    OPTIONS 
        -DENABLE_TESTS=OFF 
        -DBUILD_TOOLS=OFF
        -DWITH_POSTGRESQL=OFF
        ${GEODIFF_LINK_OPTIONS}
)

vcpkg_install_cmake()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

file(WRITE ${CURRENT_PACKAGES_DIR}/share/${PORT}/copyright "Geodiff is MIT licensed\n")
configure_file(${CMAKE_CURRENT_LIST_DIR}/vcpkg-cmake-wrapper.cmake ${CURRENT_PACKAGES_DIR}/share/${PORT}/vcpkg-cmake-wrapper.cmake @ONLY)
file(INSTALL ${CMAKE_CURRENT_LIST_DIR}/usage DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT})