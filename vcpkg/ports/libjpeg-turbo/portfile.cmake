vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO OSGeo/gdal
    REF "v${VERSION}" # keep up to date with vcpkg-overlay/gdal version
    SHA512 dfc7ccf5c1a3184fa93be762a880b7631faa4cd178cd72df8f5fd8a6296edafc56de2594617bebcb75ddf19ed4471dafcb574b22d7e9217dedfd7ea72c9247f2
    HEAD_REF master
)

# `vcpkg clean` stumbles over one subdir
file(REMOVE_RECURSE "${SOURCE_PATH}/autotest")

file(REMOVE "${SOURCE_PATH}/frmts/jpeg/libjpeg/CMakeLists.txt")
file(INSTALL ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt DESTINATION ${SOURCE_PATH}/frmts/jpeg/libjpeg/)
file(INSTALL ${CMAKE_CURRENT_LIST_DIR}/cpl_port.h DESTINATION ${SOURCE_PATH}/frmts/jpeg/)
file(INSTALL ${CMAKE_CURRENT_LIST_DIR}/libjpeg.pc.in DESTINATION ${SOURCE_PATH}/frmts/jpeg/libjpeg/)
   
if(VCPKG_LIBRARY_LINKAGE STREQUAL "static")
    list(APPEND LINK_OPTIONS -DJPEG_BUILD_TYPE="STATIC")
else()
    list(APPEND LINK_OPTIONS -DJPEG_BUILD_TYPE="SHARED")
endif()

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}/frmts/jpeg/libjpeg"
    OPTIONS
        ${LINK_OPTIONS}
)

vcpkg_cmake_install()
vcpkg_copy_pdbs()
vcpkg_fixup_pkgconfig()

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/vcpkg-cmake-wrapper.cmake" DESTINATION "${CURRENT_PACKAGES_DIR}/share/jpeg")
file(INSTALL "${SOURCE_PATH}/LICENSE.TXT" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
