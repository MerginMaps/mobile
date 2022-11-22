# GPLv2 Licence

# not in linux input-SDK

if (ANDROID)
  find_path(
    OpenSSL_INCLUDE_DIR
    openssl/ssl.h
    "${INPUT_SDK_PATH_MULTI}/include"
    NO_DEFAULT_PATH
  )

  find_library(
    OpenSSL_CRYPTO_LIBRARY
    NAMES crypto_1_1
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )

  find_library(
    OpenSSL_LIBRARY
    NAMES ssl_1_1
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )
else ()
  message(FATAL_ERROR "no OpenSSL needed for this platform")
endif ()

find_package_handle_standard_args(
  OpenSSL REQUIRED_VARS OpenSSL_LIBRARY OpenSSL_CRYPTO_LIBRARY OpenSSL_INCLUDE_DIR
)

if (OpenSSL_FOUND AND NOT TARGET OpenSSL::OpenSSL)
  add_library(OpenSSL::OpenSSL SHARED IMPORTED) # we need shared one for QT SSL modules
  set_target_properties(
    OpenSSL::OpenSSL PROPERTIES IMPORTED_LOCATION "${OpenSSL_LIBRARY}"
                                INTERFACE_INCLUDE_DIRECTORIES "${OpenSSL_INCLUDE_DIR}"
  )

  add_library(OpenSSL::Crypto SHARED IMPORTED) # we need shared one for QT SSL modules
  set_target_properties(
    OpenSSL::Crypto PROPERTIES IMPORTED_LOCATION "${OpenSSL_CRYPTO_LIBRARY}"
                               INTERFACE_INCLUDE_DIRECTORIES "${OpenSSL_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(OpenSSL_LIBRARY OpenSSL_CRYPTO_LIBRARY OpenSSL_INCLUDE_DIR)
