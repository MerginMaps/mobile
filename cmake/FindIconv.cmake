# GPLv2 Licence

# not in macos input-SDK, not in linux input-SDK

if (LNX
    OR MACOS
)
  message(FATAL_ERROR "not available for this platform in input-SDK")
  # find_library(Iconv_LIBRARY NAMES iconv)
else ()
  find_library(
    Iconv_LIBRARY
    NAMES iconv
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )
endif ()

find_package_handle_standard_args(Iconv REQUIRED_VARS Iconv_LIBRARY)

if (Iconv_FOUND AND NOT TARGET Iconv::Iconv)
  add_library(Iconv::Iconv UNKNOWN IMPORTED)
  set_target_properties(Iconv::Iconv PROPERTIES IMPORTED_LOCATION "${Iconv_LIBRARY}")

endif ()

mark_as_advanced(Iconv_LIBRARY)
