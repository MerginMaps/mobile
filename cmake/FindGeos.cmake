# GPLv2 Licence

# not in linux input-SDK

if (LINUX)
  find_path(Geos_INCLUDE_DIR NAMES geos_c.h)
  find_library(Geos_LIBRARY NAMES geos)
  find_library(Geos_c_LIBRARY NAMES geos_c)
else ()
  find_path(
    Geos_INCLUDE_DIR
    geos_c.h
    "${INPUT_SDK_PATH}/include"
    NO_DEFAULT_PATH
  )

  find_library(
    Geos_LIBRARY
    NAMES geos
    PATHS "${INPUT_SDK_PATH}/lib"
    NO_DEFAULT_PATH
  )

  find_library(
    Geos_c_LIBRARY
    NAMES geos_c
    PATHS "${INPUT_SDK_PATH}/lib"
    NO_DEFAULT_PATH
  )
endif ()

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
