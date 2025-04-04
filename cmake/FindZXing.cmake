# GPLv2 Licence

find_path(
  ZXing_INCLUDE_DIR
  ZXing/ZXVersion.h
  "${INPUT_SDK_PATH_MULTI}/include"
  NO_DEFAULT_PATH
)

find_library(
  ZXing_LIBRARY
  NAMES ZXing
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(ZXing REQUIRED_VARS ZXing_LIBRARY ZXing_INCLUDE_DIR)

if (ZXing_FOUND AND NOT TARGET ZXing::ZXing)
  add_library(ZXing::ZXing STATIC IMPORTED)
  set_target_properties(
    ZXing::ZXing PROPERTIES IMPORTED_LOCATION "${ZXing_LIBRARY}"
                            INTERFACE_INCLUDE_DIRECTORIES "${ZXing_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(ZXing_LIBRARY ZXing_INCLUDE_DIR)
