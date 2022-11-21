# GPLv2 Licence

# not in linux input-SDK

if (LINUX)
  find_path(Curl_INCLUDE_DIR NAMES curl/curl.h)
  find_library(Curl_LIBRARY NAMES curl)
else ()
  find_path(
    Curl_INCLUDE_DIR
    curl/curl.h
    "${INPUT_SDK_PATH}/include"
    NO_DEFAULT_PATH
  )

  find_library(
    Curl_LIBRARY
    NAMES curl
    PATHS "${INPUT_SDK_PATH}/lib"
    NO_DEFAULT_PATH
  )
endif ()

find_package_handle_standard_args(Curl REQUIRED_VARS Curl_LIBRARY Curl_INCLUDE_DIR)

if (Curl_FOUND AND NOT TARGET Curl::Curl)
  add_library(Curl::Curl UNKNOWN IMPORTED)
  set_target_properties(
    Curl::Curl PROPERTIES IMPORTED_LOCATION "${Curl_LIBRARY}"
                          INTERFACE_INCLUDE_DIRECTORIES "${Curl_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Curl_LIBRARY Curl_INCLUDE_DIR)
