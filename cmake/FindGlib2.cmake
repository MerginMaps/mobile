# GPLv2 Licence

if (NOT LNX)
  message(FATAL_ERROR "glib2 not available for this platform in input-SDK")
else ()

  find_library(
    Glib2_LIBRARY
    NAMES glib-2.0
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )

  find_package_handle_standard_args(Glib2 REQUIRED_VARS Glib2_LIBRARY)

  if (Glib2_FOUND AND NOT TARGET Glib2::Glib2)
    add_library(Glib2::Glib2 UNKNOWN IMPORTED)
    set_target_properties(Glib2::Glib2 PROPERTIES IMPORTED_LOCATION "${Glib2_LIBRARY}")
  endif ()

  mark_as_advanced(Glib2_LIBRARY Glib2_INCLUDE_DIR)

endif ()
