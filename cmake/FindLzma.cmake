# GPLv2 Licence

find_path(
  Lzma_INCLUDE_DIR
  lzma.h
  "${INPUT_SDK_PATH_MULTI}/include"
  NO_DEFAULT_PATH
)

find_library(
  Lzma_LIBRARY
  NAMES lzma
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(Lzma REQUIRED_VARS Lzma_LIBRARY Lzma_INCLUDE_DIR)

if (Lzma_FOUND AND NOT TARGET Lzma::Lzma)
  add_library(Lzma::Lzma UNKNOWN IMPORTED)
  set_target_properties(
    Lzma::Lzma PROPERTIES IMPORTED_LOCATION "${Lzma_LIBRARY}"
                          INTERFACE_INCLUDE_DIRECTORIES "${Lzma_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Lzma_LIBRARY Lzma_INCLUDE_DIR)
