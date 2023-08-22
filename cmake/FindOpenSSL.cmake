# GPLv2 Licence

find_path(
  OpenSSL_INCLUDE_DIR
  openssl/ssl.h
  "${INPUT_SDK_PATH_MULTI}/include"
  NO_DEFAULT_PATH
)

find_library(
  OpenSSL_CRYPTO_LIBRARY
  NAMES crypto libcrypto
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

find_library(
  OpenSSL_LIBRARY
  NAMES ssl libssl
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

if (ANDROID)
  # Android only note: on android platform we have both static openssl and shared openssl
  # libraries required for Qt shared openssl should not be linked against!, it just need
  # to be copied to APK for Qt
  find_library(
    KDAB_OpenSSL_CRYPTO_LIBRARY
    NAMES crypto_3
    PATHS "${INPUT_SDK_PATH_MULTI}/lib/kdab-openssl"
    NO_DEFAULT_PATH
  )

  find_library(
    KDAB_OpenSSL_LIBRARY
    NAMES ssl_3
    PATHS "${INPUT_SDK_PATH_MULTI}/lib/kdab-openssl"
    NO_DEFAULT_PATH
  )
endif ()

if (ANDROID)
  find_package_handle_standard_args(
    OpenSSL
    REQUIRED_VARS
      OpenSSL_LIBRARY
      OpenSSL_CRYPTO_LIBRARY
      OpenSSL_INCLUDE_DIR
      KDAB_OpenSSL_LIBRARY
      KDAB_OpenSSL_CRYPTO_LIBRARY
  )
else ()
  find_package_handle_standard_args(
    OpenSSL REQUIRED_VARS OpenSSL_LIBRARY OpenSSL_CRYPTO_LIBRARY OpenSSL_INCLUDE_DIR
  )
endif ()

if (OpenSSL_FOUND AND NOT TARGET OpenSSL::OpenSSL)
  add_library(OpenSSL::OpenSSL UNKNOWN IMPORTED)
  set_target_properties(
    OpenSSL::OpenSSL PROPERTIES IMPORTED_LOCATION "${OpenSSL_LIBRARY}"
                                INTERFACE_INCLUDE_DIRECTORIES "${OpenSSL_INCLUDE_DIR}"
  )

  add_library(OpenSSL::Crypto UNKNOWN IMPORTED)
  set_target_properties(
    OpenSSL::Crypto PROPERTIES IMPORTED_LOCATION "${OpenSSL_CRYPTO_LIBRARY}"
                               INTERFACE_INCLUDE_DIRECTORIES "${OpenSSL_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(OpenSSL_LIBRARY OpenSSL_CRYPTO_LIBRARY OpenSSL_INCLUDE_DIR)
if (ANDROID)
  mark_as_advanced(KDAB_OpenSSL_LIBRARY KDAB_OpenSSL_CRYPTO_LIBRARY)
endif ()
