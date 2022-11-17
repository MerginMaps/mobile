# GPLv2 Licence

# not in linux input-SDK

if (LINUX)
  find_path(Tiff_INCLUDE_DIR NAMES tiff.h)
  find_library(Tiff_LIBRARY NAMES tiff)
else()   
  find_path(Tiff_INCLUDE_DIR tiff.h
    "${INPUT_SDK_PATH}/include"
    NO_DEFAULT_PATH
  )
  
  find_library(Tiff_LIBRARY 
    NAMES tiff 
    PATHS "${INPUT_SDK_PATH}/lib"
    NO_DEFAULT_PATH
  )
endif()

find_package_handle_standard_args(
  Tiff
  REQUIRED_VARS Tiff_LIBRARY Tiff_INCLUDE_DIR
)

if(Tiff_FOUND AND NOT TARGET Tiff::Tiff)
  add_library(Tiff::Tiff UNKNOWN IMPORTED)
  set_target_properties(Tiff::Tiff PROPERTIES
    IMPORTED_LOCATION "${Tiff_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Tiff_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  Tiff_LIBRARY
  Tiff_INCLUDE_DIR
)