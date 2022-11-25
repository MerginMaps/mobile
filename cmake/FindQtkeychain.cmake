# GPLv2 Licence

find_path(
  Qtkeychain_INCLUDE_DIR
  qt6keychain/keychain.h
  "${INPUT_SDK_PATH_MULTI}/include"
  NO_DEFAULT_PATH
)

find_library(
  Qtkeychain_LIBRARY
  NAMES qt6keychain
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(
  Qtkeychain REQUIRED_VARS Qtkeychain_LIBRARY Qtkeychain_INCLUDE_DIR
)

if (Qtkeychain_FOUND AND NOT TARGET Qtkeychain::Qtkeychain)
  add_library(Qtkeychain::Qtkeychain STATIC IMPORTED)
  set_target_properties(
    Qtkeychain::Qtkeychain
    PROPERTIES IMPORTED_LOCATION "${Qtkeychain_LIBRARY}" INTERFACE_INCLUDE_DIRECTORIES
                                                         "${Qtkeychain_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Qtkeychain_LIBRARY Qtkeychain_INCLUDE_DIR)
