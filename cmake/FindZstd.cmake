# GPLv2 Licence

find_path(
  Zstd_INCLUDE_DIR
  zstd.h
  "${INPUT_SDK_PATH_MULTI}/include"
  NO_DEFAULT_PATH
)

find_library(
  Zstd_LIBRARY
  NAMES zstd libzstd
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(Zstd REQUIRED_VARS Zstd_LIBRARY Zstd_INCLUDE_DIR)

if (Zstd_FOUND AND NOT TARGET Zstd::Zstd)
  add_library(Zstd::Zstd UNKNOWN IMPORTED)
  set_target_properties(
    Zstd::Zstd PROPERTIES IMPORTED_LOCATION "${Zstd_LIBRARY}"
                          INTERFACE_INCLUDE_DIRECTORIES "${Zstd_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Zstd_LIBRARY Zstd_INCLUDE_DIR)
