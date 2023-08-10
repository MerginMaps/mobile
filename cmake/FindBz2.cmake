# GPLv2 Licence

find_path(Bz2_INCLUDE_DIR bzlib.h "${INPUT_SDK_PATH_MULTI}/include" NO_DEFAULT_PATH)
find_library(Bz2_LIBRARY NAMES bz2 PATHS "${INPUT_SDK_PATH_MULTI}/lib" NO_DEFAULT_PATH)

find_package_handle_standard_args(Bz2 REQUIRED_VARS Bz2_LIBRARY Bz2_INCLUDE_DIR)

if (Bz2_FOUND AND NOT TARGET Bz2::Bz2)
  add_library(Bz2::Bz2 UNKNOWN IMPORTED)
  set_target_properties(
    Bz2::Bz2 PROPERTIES IMPORTED_LOCATION "${Bz2_LIBRARY}" INTERFACE_INCLUDE_DIRECTORIES "${Bz2_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(
  Bz2_LIBRARY
  Bz2_INCLUDE_DIR
)
