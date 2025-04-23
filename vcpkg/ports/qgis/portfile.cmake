set(QGIS_REF 55d788f1f80183bd52fd8045c1fbed5e6260f194) # final-3_40_4
set(QGIS_SHA512 6eb3e2259fda72f7e2e8203d951b891963d52d47128793af02dfa989a467ba5524ab7a1019b4cf4acfd9e70c21635bd7b8577dc452d5cf7b98fcf7ce03d97b9f)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO qgis/QGIS
    REF ${QGIS_REF}
    SHA512 ${QGIS_SHA512}
    PATCHES
        cmakelists.patch
        crssync.patch
        libxml2.patch
)

file(REMOVE ${SOURCE_PATH}/cmake/FindQtKeychain.cmake)
file(REMOVE ${SOURCE_PATH}/cmake/FindGDAL.cmake)
file(REMOVE ${SOURCE_PATH}/cmake/FindGEOS.cmake)
file(REMOVE ${SOURCE_PATH}/cmake/FindEXIV2.cmake)
file(REMOVE ${SOURCE_PATH}/cmake/FindExpat.cmake)
file(REMOVE ${SOURCE_PATH}/cmake/FindIconv.cmake)

vcpkg_find_acquire_program(FLEX)
vcpkg_find_acquire_program(BISON)
vcpkg_find_acquire_program(PYTHON3)
get_filename_component(PYTHON3_PATH ${PYTHON3} DIRECTORY)
vcpkg_add_to_path(${PYTHON3_PATH})
vcpkg_add_to_path(${PYTHON3_PATH}/Scripts)
set(PYTHON_EXECUTABLE ${PYTHON3})

list(APPEND QGIS_OPTIONS -DFORCE_STATIC_LIBS:BOOL=ON)
list(APPEND QGIS_OPTIONS -DWITH_SPATIALITE:BOOL=ON)
list(APPEND QGIS_OPTIONS -DBUILD_WITH_QT6:BOOL=ON)

list(APPEND QGIS_OPTIONS -DENABLE_TESTS:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DWITH_QTWEBKIT:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DWITH_GRASS7:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DWITH_QSPATIALITE:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DUSE_OPENCL:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DWITH_BINDINGS:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DWITH_GUI:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DWITH_DESKTOP:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DWITH_CUSTOM_WIDGETS:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DWITH_SERVER_PLUGINS:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DWITH_SERVER:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DWITH_QGIS_PROCESS:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DWITH_PDAL:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DWITH_EPT:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DWITH_3D:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DWITH_DRACO:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DWITH_COPC=OFF)
list(APPEND QGIS_OPTIONS -DWITH_ANALYSIS=OFF)
list(APPEND QGIS_OPTIONS -DWITH_GRASS=OFF)
list(APPEND QGIS_OPTIONS -DWITH_GEOREFERENCER:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DWITH_QTMOBILITY:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DENABLE_TESTS=OFF)
list(APPEND QGIS_OPTIONS -DWITH_QWTPOLAR=OFF)
list(APPEND QGIS_OPTIONS -DWITH_GUI=OFF)
list(APPEND QGIS_OPTIONS -DWITH_APIDOC=OFF)
list(APPEND QGIS_OPTIONS -DWITH_ASTYLE=OFF)
list(APPEND QGIS_OPTIONS -DWITH_QT5SERIALPORT=OFF)
list(APPEND QGIS_OPTIONS -DWITH_QUICK:BOOL=OFF)
list(APPEND QGIS_OPTIONS -DQGIS_MACAPP_FRAMEWORK=FALSE)
list(APPEND QGIS_OPTIONS -DWITH_QTSERIALPORT=FALSE)

# Configure debug and release library paths
macro(FIND_LIB_OPTIONS basename relname debname suffix libsuffix)
   file(TO_NATIVE_PATH "${CURRENT_INSTALLED_DIR}/lib/${VCPKG_TARGET_IMPORT_LIBRARY_PREFIX}${relname}${libsuffix}" ${basename}_LIBRARY_RELEASE)
   file(TO_NATIVE_PATH "${CURRENT_INSTALLED_DIR}/debug/lib/${VCPKG_TARGET_IMPORT_LIBRARY_PREFIX}${debname}${libsuffix}" ${basename}_LIBRARY_DEBUG)
   if( ${basename}_LIBRARY_DEBUG AND ${basename}_LIBRARY_RELEASE AND NOT ${basename}_LIBRARY_DEBUG STREQUAL ${basename}_LIBRARY_RELEASE )
        list(APPEND QGIS_OPTIONS_RELEASE -D${basename}_${suffix}:FILEPATH=${${basename}_LIBRARY_RELEASE})
        list(APPEND QGIS_OPTIONS_DEBUG -D${basename}_${suffix}:FILEPATH=${${basename}_LIBRARY_DEBUG})
   elseif( ${basename}_LIBRARY_RELEASE )
        list(APPEND QGIS_OPTIONS -D${basename}_${suffix}:FILEPATH=${${basename}_LIBRARY_RELEASE})
   elseif( ${basename}_LIBRARY_DEBUG )
        list(APPEND QGIS_OPTIONS -D${basename}_${suffix}:FILEPATH=${${basename}_LIBRARY_DEBUG})
   endif()
endmacro()

# QGIS likes to install auth and providers to different locations on each platform
# let's keep things clean and tidy and put them at a predictable location
list(APPEND QGIS_OPTIONS -DQGIS_PLUGIN_SUBDIR=lib)

# By default QGIS installs includes into "include" on Windows and into "include/qgis" everywhere else
# let's keep things clean and tidy and put them at a predictable location
list(APPEND QGIS_OPTIONS -DQGIS_INCLUDE_SUBDIR=include/qgis)


if(VCPKG_TARGET_IS_WINDOWS)
    ##############################################################################
    #Install pip
    # if(NOT EXISTS "${PYTHON3_PATH}/Scripts/pip.exe")
    #    MESSAGE(STATUS  "Install pip for Python Begin ...")
    #    vcpkg_download_distfile(
    #        GET_PIP_PATH
    #        URLS https://bootstrap.pypa.io/pip/3.5/get-pip.py
    #        FILENAME get-pip.py
    #        SHA512  a408dd74e69a2c3259e06669562bebfff676156a1a3b2f8ba6594ed308e3f94afdf79257688e59b02bf02bd7f6ecbea510fc6665b403149031f14bcc08308a47
    #    )

    #    vcpkg_execute_required_process(
    #        COMMAND "${PYTHON_EXECUTABLE}" "${GET_PIP_PATH}"
    #        WORKING_DIRECTORY ${PYTHON3_PATH}
    #        LOGNAME pip
    #    )

    #    vcpkg_execute_required_process(
    #        COMMAND "${PYTHON_EXECUTABLE}" -m pip install --upgrade pip
    #        WORKING_DIRECTORY ${PYTHON3_PATH}
    #        LOGNAME pip
    #    )
    #    MESSAGE(STATUS  "Install pip for Python End")
    # endif (NOT EXISTS "${PYTHON3_PATH}/Scripts/pip.exe")
    ##############################################################################

    list(APPEND QGIS_OPTIONS -DBISON_EXECUTABLE="${BISON}")
    list(APPEND QGIS_OPTIONS -DFLEX_EXECUTABLE="${FLEX}")

    # list(APPEND QGIS_OPTIONS -DPYUIC_PROGRAM=${PYTHON3_PATH}/Scripts/pyuic5.exe)
    # list(APPEND QGIS_OPTIONS -DPYRCC_PROGRAM=${PYTHON3_PATH}/Scripts/pyrcc5.exe)
    # list(APPEND QGIS_OPTIONS -DQT_LRELEASE_EXECUTABLE=${CURRENT_INSTALLED_DIR}/tools/qt5-tools/bin/lrelease.exe)

    # qgis_gui depends on Qt5UiTools, and Qt5UiTools is a static library.
    # If Qt5_EXCLUDE_STATIC_DEPENDENCIES is not set, it will add the QT release library that it depends on.
    # so that in debug mode, it will reference both the qt debug library and the release library.
    # In Debug mode, add Qt5_EXCLUDE_STATIC_DEPENDENCIES to avoid this bug
    list(APPEND QGIS_OPTIONS_DEBUG -DQt5_EXCLUDE_STATIC_DEPENDENCIES:BOOL=ON)

    FIND_LIB_OPTIONS(GDAL gdal gdald LIBRARY ${VCPKG_TARGET_IMPORT_LIBRARY_SUFFIX})
    FIND_LIB_OPTIONS(GEOS geos_c geos_cd LIBRARY ${VCPKG_TARGET_IMPORT_LIBRARY_SUFFIX})
    # FIND_LIB_OPTIONS(GSL gsl gsld LIB ${VCPKG_TARGET_IMPORT_LIBRARY_SUFFIX})
    # FIND_LIB_OPTIONS(GSLCBLAS gslcblas gslcblasd LIB ${VCPKG_TARGET_IMPORT_LIBRARY_SUFFIX})
    FIND_LIB_OPTIONS(POSTGRES libpq libpq LIBRARY ${VCPKG_TARGET_IMPORT_LIBRARY_SUFFIX})
    FIND_LIB_OPTIONS(PROJ proj proj_d LIBRARY ${VCPKG_TARGET_IMPORT_LIBRARY_SUFFIX})
    # FIND_LIB_OPTIONS(PYTHON python39 python39_d LIBRARY ${VCPKG_TARGET_IMPORT_LIBRARY_SUFFIX})
    FIND_LIB_OPTIONS(QCA qca qcad LIBRARY ${VCPKG_TARGET_IMPORT_LIBRARY_SUFFIX})
    
	FIND_LIB_OPTIONS(QTKEYCHAIN qt6keychain qt6keychaind LIBRARY ${VCPKG_TARGET_IMPORT_LIBRARY_SUFFIX})
    # FIND_LIB_OPTIONS(QSCINTILLA qscintilla2_qt5 qscintilla2_qt5d LIBRARY ${VCPKG_TARGET_IMPORT_LIBRARY_SUFFIX})
    
	# if (VCPKG_LIBRARY_LINKAGE STREQUAL "static")
    #  FIND_LIB_OPTIONS(ZSTD zstd_static zstd_staticd LIBRARY ${VCPKG_TARGET_IMPORT_LIBRARY_SUFFIX})
    # endif()

    set(SPATIALINDEX_LIB_NAME spatialindex)
    if( VCPKG_TARGET_ARCHITECTURE STREQUAL "x64" OR VCPKG_TARGET_ARCHITECTURE STREQUAL "arm64" )
        set( SPATIALINDEX_LIB_NAME "spatialindex-64" )
    else()
        set( SPATIALINDEX_LIB_NAME "spatialindex-32" )
    endif()
    FIND_LIB_OPTIONS(SPATIALINDEX ${SPATIALINDEX_LIB_NAME} ${SPATIALINDEX_LIB_NAME}d LIBRARY ${VCPKG_TARGET_IMPORT_LIBRARY_SUFFIX})
endif()

list(APPEND QGIS_OPTIONS "-DWITH_INTERNAL_POLY2TRI:BOOL=OFF")

if(VCPKG_CROSSCOMPILING)
   list(APPEND QGIS_OPTIONS -DQT_HOST_PATH=${CURRENT_HOST_INSTALLED_DIR})
   list(APPEND QGIS_OPTIONS -DQT_HOST_PATH_CMAKE_DIR:PATH=${CURRENT_HOST_INSTALLED_DIR}/share)
endif()

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    #PREFER_NINJA
    OPTIONS ${QGIS_OPTIONS} 
    OPTIONS_DEBUG ${QGIS_OPTIONS_DEBUG}
    OPTIONS_RELEASE ${QGIS_OPTIONS_RELEASE}
)

vcpkg_install_cmake()

function(copy_path basepath targetdir)
    file(GLOB ${basepath}_PATH ${CURRENT_PACKAGES_DIR}/${basepath}/*)
    if( ${basepath}_PATH )
        file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/${targetdir}/${PORT}/${basepath})
        file(COPY ${${basepath}_PATH} DESTINATION ${CURRENT_PACKAGES_DIR}/${targetdir}/${PORT}/${basepath})
    endif()

    if(EXISTS "${CURRENT_PACKAGES_DIR}/${basepath}/")
        file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/${basepath}/)
    endif()
endfunction()

if(VCPKG_TARGET_IS_WINDOWS)
    file(GLOB QGIS_TOOL_PATH ${CURRENT_PACKAGES_DIR}/bin/*${VCPKG_TARGET_EXECUTABLE_SUFFIX} ${CURRENT_PACKAGES_DIR}/*${VCPKG_TARGET_EXECUTABLE_SUFFIX})
    if(QGIS_TOOL_PATH)
        file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/tools/${PORT}/bin)
        file(COPY ${QGIS_TOOL_PATH} DESTINATION ${CURRENT_PACKAGES_DIR}/tools/${PORT}/bin)
        file(REMOVE_RECURSE ${QGIS_TOOL_PATH})
        file(GLOB QGIS_TOOL_PATH ${CURRENT_PACKAGES_DIR}/bin/* )
        file(COPY ${QGIS_TOOL_PATH} DESTINATION ${CURRENT_PACKAGES_DIR}/tools/${PORT}/bin)
    endif()
    
    file(GLOB QGIS_TOOL_PATH_DEBUG ${CURRENT_PACKAGES_DIR}/debug/bin/*${VCPKG_TARGET_EXECUTABLE_SUFFIX} ${CURRENT_PACKAGES_DIR}/debug/*${VCPKG_TARGET_EXECUTABLE_SUFFIX})
    if(QGIS_TOOL_PATH_DEBUG)
        if("debug-tools" IN_LIST FEATURES)
            file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/debug/tools/${PORT}/bin)
            file(COPY ${QGIS_TOOL_PATH_DEBUG} DESTINATION ${CURRENT_PACKAGES_DIR}/debug/tools/${PORT}/bin)
            file(REMOVE_RECURSE ${QGIS_TOOL_PATH_DEBUG})
            file(GLOB QGIS_TOOL_PATH_DEBUG ${CURRENT_PACKAGES_DIR}/debug/bin/* )
            file(COPY ${QGIS_TOOL_PATH_DEBUG} DESTINATION ${CURRENT_PACKAGES_DIR}/debug/tools/${PORT}/bin)
        else()
            file(REMOVE_RECURSE ${QGIS_TOOL_PATH_DEBUG})
        endif()
    endif()

    copy_path(doc share)
    copy_path(i18n share)
    copy_path(icons share)
    copy_path(images share)
    copy_path(plugins tools)
    copy_path(resources share)
    copy_path(svg share)
    
    # Extend vcpkg_copy_tool_dependencies to support the export of dll and exe dependencies in different directories to the same directory,
    # and support the copy of debug dependencies
    function(vcpkg_copy_tool_dependencies_ex TOOL_DIR OUTPUT_DIR SEARCH_DIR)
        find_program(PS_EXE powershell PATHS ${DOWNLOADS}/tool)
        if (PS_EXE-NOTFOUND)
            message(FATAL_ERROR "Could not find powershell in vcpkg tools, please open an issue to report this.")
        endif()
        macro(search_for_dependencies PATH_TO_SEARCH)
            file(GLOB TOOLS ${TOOL_DIR}/*.exe ${TOOL_DIR}/*.dll)
            foreach(TOOL ${TOOLS})
                vcpkg_execute_required_process(
                    COMMAND ${PS_EXE} -noprofile -executionpolicy Bypass -nologo
                        -file ${CMAKE_CURRENT_LIST_DIR}/applocal.ps1
                        -targetBinary ${TOOL}
                        -installedDir ${PATH_TO_SEARCH}
                        -outputDir    ${OUTPUT_DIR}
                    WORKING_DIRECTORY ${VCPKG_ROOT_DIR}
                    LOGNAME copy-tool-dependencies
                )
            endforeach()
        endmacro()
        search_for_dependencies(${CURRENT_PACKAGES_DIR}/${SEARCH_DIR})
        search_for_dependencies(${CURRENT_INSTALLED_DIR}/${SEARCH_DIR})
    endfunction()

    vcpkg_copy_tool_dependencies_ex(${CURRENT_PACKAGES_DIR}/tools/${PORT}/bin ${CURRENT_PACKAGES_DIR}/tools/${PORT}/bin bin)
    vcpkg_copy_tool_dependencies_ex(${CURRENT_PACKAGES_DIR}/tools/${PORT}/plugins ${CURRENT_PACKAGES_DIR}/tools/${PORT}/bin bin)
    if("debug-tools" IN_LIST FEATURES)
        vcpkg_copy_tool_dependencies_ex(${CURRENT_PACKAGES_DIR}/debug/tools/${PORT}/bin ${CURRENT_PACKAGES_DIR}/debug/tools/${PORT}/bin debug/bin)
        vcpkg_copy_tool_dependencies_ex(${CURRENT_PACKAGES_DIR}/debug/tools/${PORT}/plugins ${CURRENT_PACKAGES_DIR}/debug/tools/${PORT}/bin debug/bin)
    endif()
    if("server" IN_LIST FEATURES)
        vcpkg_copy_tool_dependencies_ex(${CURRENT_PACKAGES_DIR}/tools/${PORT}/server ${CURRENT_PACKAGES_DIR}/tools/${PORT}/bin bin)
        if("debug-tools" IN_LIST FEATURES)
            vcpkg_copy_tool_dependencies_ex(${CURRENT_PACKAGES_DIR}/debug/tools/${PORT}/server ${CURRENT_PACKAGES_DIR}/debug/tools/${PORT}/bin debug/bin)
        endif()
    endif()

    list(APPEND QGIS_OPTIONS -DWITH_INTERNAL_POLY2TRI=ON)
    file(REMOVE ${SOURCE_PATH}/cmake/FindPoly2Tri.cmake)
else()
    list(APPEND QGIS_OPTIONS -DWITH_INTERNAL_POLY2TRI=OFF)
    if(EXISTS "${CURRENT_INSTALLED_DIR}/lib/libqt_poly2tri.a")
        set(QT_POLY2TRI_DIR_RELEASE "${CURRENT_INSTALLED_DIR}/lib")
        set(QT_POLY2TRI_DIR_DEBUG "${CURRENT_INSTALLED_DIR}/debug/lib")
    elseif(EXISTS "${Qt6_DIR}/../../libqt_poly2tri.a")
        set(QT_POLY2TRI_DIR_RELEASE "${Qt6_DIR}/../..")
        set(QT_POLY2TRI_DIR_DEBUG "${Qt6_DIR}/../..")
    else()
        list(APPEND QGIS_OPTIONS -DPoly2Tri_LIBRARY=poly2tri::poly2tri)
    endif()
    if(DEFINED QT_POLY2TRI_DIR_RELEASE)
        list(APPEND QGIS_OPTIONS -DPoly2Tri_INCLUDE_DIR:PATH=${CMAKE_CURRENT_LIST_DIR}/poly2tri)
        list(APPEND QGIS_OPTIONS_DEBUG -DPoly2Tri_LIBRARY:PATH=${QT_POLY2TRI_DIR_DEBUG}/debug/lib/libqt_poly2tri_debug.a) # static qt only
        list(APPEND QGIS_OPTIONS_RELEASE -DPoly2Tri_LIBRARY:PATH=${QT_POLY2TRI_DIR_RELEASE}/lib/libqt_poly2tri.a) # static qt only
    endif()
endif()

if(VCPKG_TARGET_IS_OSX)
    copy_path(resources share)
endif()

file(GLOB QGIS_CMAKE_PATH ${CURRENT_PACKAGES_DIR}/*.cmake)
if(QGIS_CMAKE_PATH)
    file(COPY ${QGIS_CMAKE_PATH} DESTINATION ${CURRENT_PACKAGES_DIR}/share/cmake/${PORT})
    file(REMOVE_RECURSE ${QGIS_CMAKE_PATH})
endif()

file(GLOB QGIS_CMAKE_PATH_DEBUG ${CURRENT_PACKAGES_DIR}/debug/*.cmake)
if( QGIS_CMAKE_PATH_DEBUG )
    file(REMOVE_RECURSE ${QGIS_CMAKE_PATH_DEBUG})
endif()

file(REMOVE_RECURSE
    ${CURRENT_PACKAGES_DIR}/debug/include
)
file(REMOVE_RECURSE # Added for debug porpose
    ${CURRENT_PACKAGES_DIR}/debug/share
)

# Handle copyright
file(INSTALL ${SOURCE_PATH}/COPYING DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)