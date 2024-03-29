# GPLv2 Licence

find_path(
  Spatialite_INCLUDE_DIR
  spatialite.h
  "${INPUT_SDK_PATH_MULTI}/include"
  NO_DEFAULT_PATH
)

find_library(
  Spatialite_LIBRARY
  NAMES spatialite
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(
  Spatialite REQUIRED_VARS Spatialite_LIBRARY Spatialite_INCLUDE_DIR
)

if (Spatialite_FOUND AND NOT TARGET Spatialite::Spatialite)
  add_library(Spatialite::Spatialite UNKNOWN IMPORTED)
  set_target_properties(
    Spatialite::Spatialite
    PROPERTIES IMPORTED_LOCATION "${Spatialite_LIBRARY}" INTERFACE_INCLUDE_DIRECTORIES
                                                         "${Spatialite_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Spatialite_LIBRARY Spatialite_INCLUDE_DIR)
