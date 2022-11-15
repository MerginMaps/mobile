# GPLv2 Licence

find_path(Expat_INCLUDE_DIR expat.h
  "${INPUT_SDK_PATH}/include"
  NO_DEFAULT_PATH
)
  
find_library(Expat_LIBRARY 
  NAMES expat 
  PATHS "${INPUT_SDK_PATH}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(
  Expat
  REQUIRED_VARS Expat_LIBRARY Expat_INCLUDE_DIR
)

if(Expat_FOUND AND NOT TARGET Expat::Expat)
  add_library(Expat::Expat STATIC IMPORTED)
  set_target_properties(Expat::Expat PROPERTIES
    IMPORTED_LOCATION "${Expat_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Expat_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  Expat_LIBRARY
  Expat_INCLUDE_DIR
)