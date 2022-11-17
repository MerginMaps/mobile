# GPLv2 Licence

# not in linux input-SDK

if (LINUX)
  find_path(Spatialite_INCLUDE_DIR NAMES spatialite.h)
  find_library(Spatialite_LIBRARY NAMES spatialite)
else()    
  find_path(Spatialite_INCLUDE_DIR spatialite.h
    "${INPUT_SDK_PATH}/include"
    NO_DEFAULT_PATH
  )
  
  find_library(Spatialite_LIBRARY 
    NAMES spatialite 
    PATHS "${INPUT_SDK_PATH}/lib"
    NO_DEFAULT_PATH
  )
endif()

find_package_handle_standard_args(
  Spatialite
  REQUIRED_VARS Spatialite_LIBRARY Spatialite_INCLUDE_DIR
)

if(Spatialite_FOUND AND NOT TARGET Spatialite::Spatialite)
  add_library(Spatialite::Spatialite STATIC IMPORTED)
  set_target_properties(Spatialite::Spatialite PROPERTIES
    IMPORTED_LOCATION "${Spatialite_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Spatialite_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  Spatialite_LIBRARY
  Spatialite_INCLUDE_DIR
)