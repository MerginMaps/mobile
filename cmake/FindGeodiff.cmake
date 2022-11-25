# GPLv2 Licence

find_path(
  Geodiff_INCLUDE_DIR
  geodiff.h
  "${INPUT_SDK_PATH_MULTI}/include"
  NO_DEFAULT_PATH
)

find_library(
  Geodiff_LIBRARY
  NAMES geodiff
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(
  Geodiff REQUIRED_VARS Geodiff_LIBRARY Geodiff_INCLUDE_DIR
)

if (Geodiff_FOUND AND NOT TARGET Geodiff::Geodiff)
  add_library(Geodiff::Geodiff STATIC IMPORTED)
  set_target_properties(
    Geodiff::Geodiff PROPERTIES IMPORTED_LOCATION "${Geodiff_LIBRARY}"
                                INTERFACE_INCLUDE_DIRECTORIES "${Geodiff_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Geodiff_LIBRARY Geodiff_INCLUDE_DIR)
