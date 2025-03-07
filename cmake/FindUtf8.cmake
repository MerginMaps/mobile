# GPLv2 Licence

find_library(
  utf8_range_LIBRARY
  NAMES libutf8_range utf8_range
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

find_library(
  utf8_validity_LIBRARY
  NAMES libutf8_validity utf8_validity
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(
  Utf8 REQUIRED_VARS utf8_range_LIBRARY utf8_validity_LIBRARY
)

if (Utf8_FOUND AND NOT TARGET Utf8::utf8_range)
  add_library(Utf8::utf8_range UNKNOWN IMPORTED)
  set_target_properties(
    Utf8::utf8_range
    PROPERTIES IMPORTED_LOCATION "${utf8_range_LIBRARY}"
  )
endif ()

if (Utf8_FOUND AND NOT TARGET Utf8::utf8_validity)
  add_library(Utf8::utf8_validity UNKNOWN IMPORTED)
  set_target_properties(
    Utf8::utf8_validity
    PROPERTIES IMPORTED_LOCATION "${utf8_validity_LIBRARY}"
  )
endif ()

mark_as_advanced(utf8_range_LIBRARY utf8_validity_LIBRARY)
