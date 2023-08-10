# GPLv2 Licence

  find_path(
    Tiff_INCLUDE_DIR
    tiff.h
    "${INPUT_SDK_PATH_MULTI}/include"
    NO_DEFAULT_PATH
  )

  find_library(
    Tiff_LIBRARY
    NAMES tiff
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )

find_package_handle_standard_args(Tiff REQUIRED_VARS Tiff_LIBRARY Tiff_INCLUDE_DIR)

if (Tiff_FOUND AND NOT TARGET Tiff::Tiff)
  add_library(Tiff::Tiff UNKNOWN IMPORTED)
  set_target_properties(
    Tiff::Tiff PROPERTIES IMPORTED_LOCATION "${Tiff_LIBRARY}"
                          INTERFACE_INCLUDE_DIRECTORIES "${Tiff_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Tiff_LIBRARY Tiff_INCLUDE_DIR)
