# GPLv2 Licence

# not in macos and linux input-SDK
if (MACOS OR LNX)
  # find_library(Charset_LIBRARY NAMES charset)
  message(FATAL_ERROR "not available for this platform in input-SDK")
else ()
  find_library(
    Charset_LIBRARY
    NAMES charset
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )
endif ()

find_package_handle_standard_args(Charset REQUIRED_VARS Charset_LIBRARY)

if (Charset_FOUND AND NOT TARGET Charset::Charset)
  add_library(Charset::Charset UNKNOWN IMPORTED)
  set_target_properties(
    Charset::Charset PROPERTIES IMPORTED_LOCATION "${Charset_LIBRARY}"
  )

endif ()

mark_as_advanced(Charset_LIBRARY)
