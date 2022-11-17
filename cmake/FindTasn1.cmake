# GPLv2 Licence

# not in linux input-SDK

if (LINUX)
  # find_path(Tasn1_INCLUDE_DIR NAMES libtasn1.h)
  # find_library(Tasn1_LIBRARY NAMES tasn1)
  message(FATAL_ERROR "tasn1 is not needed on LINUX")
else()   
  find_path(Tasn1_INCLUDE_DIR libtasn1.h
    "${INPUT_SDK_PATH}/include"
    NO_DEFAULT_PATH
  )
  
  find_library(Tasn1_LIBRARY 
    NAMES tasn1 
    PATHS "${INPUT_SDK_PATH}/lib"
    NO_DEFAULT_PATH
  )
endif()

find_package_handle_standard_args(
  Tasn1
  REQUIRED_VARS Tasn1_LIBRARY Tasn1_INCLUDE_DIR
)

if(Tasn1_FOUND AND NOT TARGET Tasn1::Tasn1)
  add_library(Tasn1::Tasn1 STATIC IMPORTED)
  set_target_properties(Tasn1::Tasn1 PROPERTIES
    IMPORTED_LOCATION "${Tasn1_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Tasn1_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  Tasn1_LIBRARY
  Tasn1_INCLUDE_DIR
)