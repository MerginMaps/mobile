# GPLv2 Licence

find_library(ZLIB_LIBRARY NAMES z zlib libz)

find_package_handle_standard_args(ZLIB REQUIRED_VARS ZLIB_LIBRARY)

if (ZLIB_FOUND AND NOT TARGET ZLIB::ZLIB)
  add_library(ZLIB::ZLIB UNKNOWN IMPORTED)
  set_target_properties(ZLIB::ZLIB PROPERTIES IMPORTED_LOCATION "${ZLIB_LIBRARY}")
endif ()

mark_as_advanced(ZLIB_LIBRARY)
