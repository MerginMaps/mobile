# GPLv2 Licence

# not in linux input-SDK

if (LINUX)
  find_path(Exiv2_INCLUDE_DIR NAMES exif.hpp)
  find_library(Exiv2_LIBRARY NAMES exiv2)
  find_library(Exiv2_xmp_LIBRARY NAMES exiv2-xmp)
else()
  find_path(Exiv2_INCLUDE_DIR exif.hpp
    "${INPUT_SDK_PATH}/include/exiv2"
    NO_DEFAULT_PATH
  ) 
  
  find_library(Exiv2_LIBRARY 
    NAMES exiv2 
    PATHS "${INPUT_SDK_PATH}/lib"
    NO_DEFAULT_PATH
  )

  find_library(Exiv2_xmp_LIBRARY 
    NAMES exiv2-xmp 
    PATHS "${INPUT_SDK_PATH}/lib"
    NO_DEFAULT_PATH
  )
endif()

find_package_handle_standard_args(
  Exiv2
  REQUIRED_VARS Exiv2_LIBRARY Exiv2_xmp_LIBRARY Exiv2_INCLUDE_DIR
)

if(Exiv2_FOUND AND NOT TARGET Exiv2::Exiv2)
  add_library(Exiv2::Exiv2 UNKNOWN IMPORTED)
  set_target_properties(Exiv2::Exiv2 PROPERTIES
    IMPORTED_LOCATION "${Exiv2_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Exiv2_INCLUDE_DIR}"
  )
  
  add_library(Exiv2::Exiv2-xmp UNKNOWN IMPORTED)
  set_target_properties(Exiv2::Exiv2-xmp PROPERTIES
    IMPORTED_LOCATION "${Exiv2_xmp_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Exiv2_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  Exiv2_LIBRARY
  Exiv2_INCLUDE_DIR
  Exiv2_xmp_LIBRARY
)