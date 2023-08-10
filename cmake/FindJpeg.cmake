# GPLv2 Licence

# GDAL uses internal JPEG with renamed symbols
# TIFF uses jpegturbo
# QT has bundled Qt6BundledLibjpeg on iOS

find_path(
    Jpeg_INCLUDE_DIR
    jpeglib.h
    "${INPUT_SDK_PATH_MULTI}/include"
    NO_DEFAULT_PATH
  )

find_library(
    Jpeg_LIBRARY
    NAMES jpeg
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
)

find_package_handle_standard_args(Jpeg REQUIRED_VARS Jpeg_LIBRARY Jpeg_INCLUDE_DIR)

if (Jpeg_FOUND AND NOT TARGET Jpeg::Jpeg)
  add_library(Jpeg::Jpeg UNKNOWN IMPORTED)
  set_target_properties(
    Jpeg::Jpeg PROPERTIES IMPORTED_LOCATION "${Jpeg_LIBRARY}"
                          INTERFACE_INCLUDE_DIRECTORIES "${Jpeg_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Jpeg_LIBRARY Jpeg_INCLUDE_DIR)
