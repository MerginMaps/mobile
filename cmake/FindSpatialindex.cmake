# GPLv2 Licence

  find_path(
    Spatialindex_INCLUDE_DIR
    spatialindex/RTree.h
    "${INPUT_SDK_PATH_MULTI}/include"
    NO_DEFAULT_PATH
  )

  find_library(
    Spatialindex_LIBRARY
    NAMES spatialindex spatialindex-64
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )

find_package_handle_standard_args(
  Spatialindex REQUIRED_VARS Spatialindex_LIBRARY Spatialindex_INCLUDE_DIR
)

if (Spatialindex_FOUND AND NOT TARGET Spatialindex::Spatialindex)
  add_library(Spatialindex::Spatialindex UNKNOWN IMPORTED)
  set_target_properties(
    Spatialindex::Spatialindex
    PROPERTIES IMPORTED_LOCATION "${Spatialindex_LIBRARY}" INTERFACE_INCLUDE_DIRECTORIES
                                                           "${Spatialindex_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Spatialindex_LIBRARY Spatialindex_INCLUDE_DIR)
