# GPLv2 Licence

# not in linux input-SDK

if (LINUX)
  find_path(WebP_INCLUDE_DIR NAMES webp/encode.h)
  find_library(WebP_LIBRARY NAMES webp)
else ()
  find_path(
    WebP_INCLUDE_DIR
    webp/encode.h
    "${INPUT_SDK_PATH}/include"
    NO_DEFAULT_PATH
  )

  find_library(
    WebP_LIBRARY
    NAMES webp
    PATHS "${INPUT_SDK_PATH}/lib"
    NO_DEFAULT_PATH
  )
endif ()

find_package_handle_standard_args(WebP REQUIRED_VARS WebP_LIBRARY WebP_INCLUDE_DIR)

if (WebP_FOUND AND NOT TARGET WebP::WebP)
  add_library(WebP::WebP UNKNOWN IMPORTED)
  set_target_properties(
    WebP::WebP PROPERTIES IMPORTED_LOCATION "${WebP_LIBRARY}"
                          INTERFACE_INCLUDE_DIRECTORIES "${WebP_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(WebP_LIBRARY WebP_INCLUDE_DIR)
