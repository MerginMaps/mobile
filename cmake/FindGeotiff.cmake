# GPLv2 Licence

find_path(
  Geotiff_INCLUDE_DIR
  geotiff.h
  "${INPUT_SDK_PATH_MULTI}/include"
  NO_DEFAULT_PATH
)

find_library(
  Geotiff_LIBRARY
  NAMES geotiff libgeotiff
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(
  Geotiff REQUIRED_VARS Geotiff_LIBRARY Geotiff_INCLUDE_DIR
)

if (Geotiff_FOUND AND NOT TARGET Geotiff::Geotiff)
  add_library(Geotiff::Geotiff UNKNOWN IMPORTED)
  set_target_properties(
    Geotiff::Geotiff PROPERTIES IMPORTED_LOCATION "${Geotiff_LIBRARY}"
                                INTERFACE_INCLUDE_DIRECTORIES "${Geotiff_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Geotiff_LIBRARY Geotiff_INCLUDE_DIR)
