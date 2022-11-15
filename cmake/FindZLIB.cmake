# GPLv2 Licence

# not in macos input-SDK

# find_path(ZLIB_INCLUDE_DIR tiff.h
#  "${INPUT_SDK_PATH}/include"
#  NO_DEFAULT_PATH
#)
  
find_library(ZLIB_LIBRARY 
  NAMES z
)

find_package_handle_standard_args(
  ZLIB
  REQUIRED_VARS ZLIB_LIBRARY # ZLIB_INCLUDE_DIR
)

if(ZLIB_FOUND AND NOT TARGET ZLIB::ZLIB)
  add_library(ZLIB::ZLIB STATIC IMPORTED)
  set_target_properties(ZLIB::ZLIB PROPERTIES
    IMPORTED_LOCATION "${ZLIB_LIBRARY}"
#    INTERFACE_INCLUDE_DIRECTORIES "${ZLIB_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  ZLIB_LIBRARY
#  ZLIB_INCLUDE_DIR
)