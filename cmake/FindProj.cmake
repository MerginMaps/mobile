# GPLv2 Licence

find_path(Proj_INCLUDE_DIR geodiff.h
  "${INPUT_SDK_PATH}/include"
  NO_DEFAULT_PATH
)
  
find_library(Proj_LIBRARY 
  NAMES proj 
  PATHS "${INPUT_SDK_PATH}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(
  Proj
  REQUIRED_VARS Proj_LIBRARY Proj_INCLUDE_DIR
)

if(Proj_FOUND AND NOT TARGET Proj::Proj)
  add_library(Proj::Proj STATIC IMPORTED)
  set_target_properties(Proj::Proj PROPERTIES
    IMPORTED_LOCATION "${Proj_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Proj_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  Proj_LIBRARY
  Proj_INCLUDE_DIR
)