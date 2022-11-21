# GPLv2 Licence

# not in macos input-SDK not in linux input-SDK

if (LINUX OR MACOS)
  # find_path(Geos_INCLUDE_DIR NAMES geos_c.h)
  find_library(Iconv_LIBRARY NAMES iconv)
else ()

  # TODO do we need this? find_path(Iconv_INCLUDE_DIR tiff.h "${INPUT_SDK_PATH}/include"
  # NO_DEFAULT_PATH )

  find_library(
    Iconv_LIBRARY
    NAMES iconv
    PATHS "${INPUT_SDK_PATH}/lib"
    NO_DEFAULT_PATH
  )
endif ()

find_package_handle_standard_args(Iconv REQUIRED_VARS Iconv_LIBRARY # Iconv_INCLUDE_DIR
)

if (Iconv_FOUND AND NOT TARGET Iconv::Iconv)
  add_library(Iconv::Iconv UNKNOWN IMPORTED)
  set_target_properties(
    Iconv::Iconv
    PROPERTIES IMPORTED_LOCATION "${Iconv_LIBRARY}" # INTERFACE_INCLUDE_DIRECTORIES
                                                    # "${Iconv_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Iconv_LIBRARY # Iconv_INCLUDE_DIR
)
