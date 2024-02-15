# GPLv2 Licence

find_path(
  MiniZip_INCLUDE_DIR
  minizip/zip.h
  "${INPUT_SDK_PATH_MULTI}/include"
  NO_DEFAULT_PATH
)

find_library(
  MiniZip_LIBRARY
  NAMES minizip
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(
  MiniZip REQUIRED_VARS MiniZip_LIBRARY MiniZip_INCLUDE_DIR
)

if (MiniZip_FOUND AND NOT TARGET MiniZip::MiniZip)
  add_library(MiniZip::MiniZip UNKNOWN IMPORTED)
  set_target_properties(
    MiniZip::MiniZip PROPERTIES IMPORTED_LOCATION "${MiniZip_LIBRARY}"
                                INTERFACE_INCLUDE_DIRECTORIES "${MiniZip_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(MiniZip_LIBRARY MiniZip_INCLUDE_DIR)
