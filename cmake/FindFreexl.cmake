# GPLv2 Licence

# not in linux input-SDK

if (LINUX)
  find_path(Freexl_INCLUDE_DIR NAMES freexl.h)
  find_library(Freexl_LIBRARY NAMES freexl)
else()  
  find_path(Freexl_INCLUDE_DIR freexl.h
    "${INPUT_SDK_PATH}/include"
    NO_DEFAULT_PATH
  )
  
  find_library(Freexl_LIBRARY 
    NAMES freexl 
    PATHS "${INPUT_SDK_PATH}/lib"
    NO_DEFAULT_PATH
  )
endif()

find_package_handle_standard_args(
  Freexl
  REQUIRED_VARS Freexl_LIBRARY Freexl_INCLUDE_DIR
)

if(Freexl_FOUND AND NOT TARGET Freexl::Freexl)
  add_library(Freexl::Freexl UNKNOWN IMPORTED)
  set_target_properties(Freexl::Freexl PROPERTIES
    IMPORTED_LOCATION "${Freexl_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Freexl_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  Freexl_LIBRARY
  Freexl_INCLUDE_DIR
)