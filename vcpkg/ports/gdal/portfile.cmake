vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO OSGeo/gdal
    REF "v${VERSION}" # keep up to date with vcpkg-overlay/libjpeg-turbo version
    SHA512 c974977789092058540ace0e9192a17b0f593e770822a885181ec6eed4323e2f7a7a61963667ae8c9c8c8c0d2d8fb80d194ab4d918e6d4851f39abd1e1c948bc
    HEAD_REF master
    PATCHES
        find-link-libraries.patch
        fix-gdal-target-interfaces.patch
        libkml.patch
        target-is-valid.patch
)

# `vcpkg clean` stumbles over one subdir
file(REMOVE_RECURSE "${SOURCE_PATH}/autotest")

# Avoid abseil, no matter if vcpkg or system
vcpkg_replace_string("${SOURCE_PATH}/ogr/ogrsf_frmts/flatgeobuf/flatbuffers/base.h" [[__has_include("absl/strings/string_view.h")]] "(0)")

list(APPEND FEATURE_OPTIONS -DGDAL_USE_PNG_INTERNAL=ON)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_JSONC_INTERNAL=ON)

list(APPEND FEATURE_OPTIONS -DGDAL_USE_ICONV=ON)
if(GDAL_USE_ICONV AND VCPKG_TARGET_IS_WINDOWS)
    list(APPEND FEATURE_OPTIONS -D_ICONV_SECOND_ARGUMENT_IS_NOT_CONST=ON)
endif()

list(APPEND FEATURE_OPTIONS -DGDAL_USE_LIBXML2=ON)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_TIFF=ON)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_CURL=ON)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_WEBP=ON)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_SQLITE3=ON)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_GEOS=ON)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_GEOTIFF=ON)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_SPATIALITE=ON)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_EXPAT=ON)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_ZLIB=ON)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_FREEXL=ON)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_JPEG=ON)


list(APPEND FEATURE_OPTIONS -DGDAL_USE_KEA=OFF)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_POSTGRESQL=OFF)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_PCRE2=OFF)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_GIF=OFF)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_OPENJPEG=OFF)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_ODBC=OFF)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_CFITSIO=OFF)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_HDF5=OFF)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_MYSQL=OFF)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_POPPLER=OFF)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_ODBC=OFF)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_MSSQL_ODBC=OFF)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_ZSTD=OFF)
list(APPEND FEATURE_OPTIONS -DGDAL_USE_LIBKML=OFF)

# Compatibility with older Android versions https://github.com/OSGeo/gdal/pull/5941
if(VCPKG_TARGET_IS_ANDROID AND ANRDOID_PLATFORM VERSION_LESS 24 AND (VCPKG_TARGET_ARCHITECTURE STREQUAL "x86" OR VCPKG_TARGET_ARCHITECTURE STREQUAL "arm"))
    list(APPEND FEATURE_OPTIONS -DBUILD_WITHOUT_64BIT_OFFSET=ON)
endif()

# we want gdalinfo/ogrinfo on macOS to generate supported formats list
if(VCPKG_TARGET_IS_OSX)
    list(APPEND FEATURE_OPTIONS -DBUILD_APPS=ON)
else()
    list(APPEND FEATURE_OPTIONS -DBUILD_APPS=OFF)
endif()

string(REPLACE "dynamic" "" qhull_target "Qhull::qhull${VCPKG_LIBRARY_LINKAGE}_r")

# PCIDSK does not add compile interface defininions!
# do this instead of patching for now
# https://github.com/OSGeo/gdal/blob/af5b75ecc6b8d3cef36f2b6fecf085319d39a546/frmts/pcidsk/sdk/CMakeLists.txt#L84
set(VCPKG_C_FLAGS "${VCPKG_C_FLAGS} -DRENAME_INTERNAL_LIBJPEG_SYMBOLS")
set(VCPKG_CXX_FLAGS "${VCPKG_CXX_FLAGS} -DRENAME_INTERNAL_LIBJPEG_SYMBOLS")

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DVCPKG_HOST_TRIPLET=${HOST_TRIPLET} # for host pkgconf in PATH
        ${FEATURE_OPTIONS}
        -DBUILD_DOCS=OFF
        -DBUILD_PYTHON_BINDINGS=OFF
        -DBUILD_TESTING=OFF
        -DCMAKE_DISABLE_FIND_PACKAGE_CSharp=ON
        -DCMAKE_DISABLE_FIND_PACKAGE_Java=ON
        -DCMAKE_DISABLE_FIND_PACKAGE_JNI=ON
        -DCMAKE_DISABLE_FIND_PACKAGE_SWIG=ON
        -DGDAL_USE_INTERNAL_LIBS=OFF
        -DGDAL_USE_EXTERNAL_LIBS=OFF
        -DGDAL_BUILD_OPTIONAL_DRIVERS=ON
        -DOGR_BUILD_OPTIONAL_DRIVERS=ON
        -DGDAL_CHECK_PACKAGE_MySQL_NAMES=unofficial-libmariadb
        -DGDAL_CHECK_PACKAGE_MySQL_TARGETS=unofficial::libmariadb
        -DMYSQL_LIBRARIES=unofficial::libmariadb
        -DGDAL_CHECK_PACKAGE_NetCDF_NAMES=netCDF
        -DGDAL_CHECK_PACKAGE_NetCDF_TARGETS=netCDF::netcdf
        -DGDAL_CHECK_PACKAGE_QHULL_NAMES=Qhull
        "-DGDAL_CHECK_PACKAGE_QHULL_TARGETS=${qhull_target}"
        "-DQHULL_LIBRARY=${qhull_target}"
        -DCMAKE_PROJECT_INCLUDE="${CMAKE_CURRENT_LIST_DIR}/cmake-project-include.cmake"
    OPTIONS_DEBUG
        -DBUILD_APPS=OFF
    MAYBE_UNUSED_VARIABLES
        QHULL_LIBRARY
)

vcpkg_cmake_install()
vcpkg_copy_pdbs()
vcpkg_fixup_pkgconfig()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/gdal)
vcpkg_replace_string("${CURRENT_PACKAGES_DIR}/share/gdal/GDALConfig.cmake"
    "include(CMakeFindDependencyMacro)"
    "include(CMakeFindDependencyMacro)
# gdal needs a pkg-config tool. A host dependency provides pkgconf.
get_filename_component(vcpkg_host_prefix \"\${CMAKE_CURRENT_LIST_DIR}/../../../${HOST_TRIPLET}\" ABSOLUTE)
list(APPEND CMAKE_PROGRAM_PATH \"\${vcpkg_host_prefix}/tools/pkgconf\")"
)

if(VCPKG_TARGET_IS_OSX)
    vcpkg_copy_tools(
        TOOL_NAMES
            gdalinfo
            ogrinfo
        AUTO_CLEAN
    )
endif()

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/include"
    "${CURRENT_PACKAGES_DIR}/debug/share"
)

file(REMOVE "${CURRENT_PACKAGES_DIR}/bin/gdal-config" "${CURRENT_PACKAGES_DIR}/debug/bin/gdal-config")

file(GLOB bin_files "${CURRENT_PACKAGES_DIR}/bin/*")
if(NOT bin_files)
    file(REMOVE_RECURSE
        "${CURRENT_PACKAGES_DIR}/bin"
        "${CURRENT_PACKAGES_DIR}/debug/bin"
    )
endif()

vcpkg_replace_string("${CURRENT_PACKAGES_DIR}/include/cpl_config.h" "#define GDAL_PREFIX \"${CURRENT_PACKAGES_DIR}\"" "")

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/vcpkg-cmake-wrapper.cmake" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
file(INSTALL "${SOURCE_PATH}/LICENSE.TXT" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
