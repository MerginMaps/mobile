# GPLv2 Licence

find_path(
  Gdal_INCLUDE_DIR
  gdal.h
  "${INPUT_SDK_PATH_MULTI}/include"
  NO_DEFAULT_PATH
)

find_library(
  Gdal_LIBRARY
  NAMES gdal
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(Gdal REQUIRED_VARS Gdal_LIBRARY Gdal_INCLUDE_DIR)

if (Gdal_FOUND AND NOT TARGET Gdal::Gdal)
  add_library(Gdal::Gdal STATIC IMPORTED)
  set_target_properties(
    Gdal::Gdal PROPERTIES IMPORTED_LOCATION "${Gdal_LIBRARY}"
                          INTERFACE_INCLUDE_DIRECTORIES "${Gdal_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Gdal_LIBRARY Gdal_INCLUDE_DIR)
