# GPLv2 Licence

  find_path(
    Geos_INCLUDE_DIR
    geos_c.h
    "${INPUT_SDK_PATH_MULTI}/include"
    NO_DEFAULT_PATH
  )

  find_library(
    Geos_LIBRARY
    NAMES geos
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )

  find_library(
    Geos_c_LIBRARY
    NAMES geos_c
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )

find_package_handle_standard_args(
  Geos REQUIRED_VARS Geos_LIBRARY Geos_c_LIBRARY Geos_INCLUDE_DIR
)

if (Geos_FOUND AND NOT TARGET Geos::Geos)
  add_library(Geos::Geos UNKNOWN IMPORTED)
  set_target_properties(
    Geos::Geos PROPERTIES IMPORTED_LOCATION "${Geos_LIBRARY}"
                          INTERFACE_INCLUDE_DIRECTORIES "${Geos_INCLUDE_DIR}"
  )

  add_library(Geos::Geos-c UNKNOWN IMPORTED)
  set_target_properties(
    Geos::Geos-c PROPERTIES IMPORTED_LOCATION "${Geos_c_LIBRARY}"
                            INTERFACE_INCLUDE_DIRECTORIES "${Geos_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Geos_LIBRARY Geos_INCLUDE_DIR Geos_c_LIBRARY)
