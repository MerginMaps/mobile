# GPLv2 Licence

if (WIN)
  find_path(
    Qca_INCLUDE_DIR
    QtCrypto/qca.h
    "${INPUT_SDK_PATH_MULTI}/include"
    NO_DEFAULT_PATH
  )

  find_library(
    Qca_LIBRARY
    NAMES qca
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )
else ()
  find_path(
    Qca_INCLUDE_DIR
    Qca-qt6/QtCrypto/qca.h
    "${INPUT_SDK_PATH_MULTI}/include"
    NO_DEFAULT_PATH
  )

  find_library(
    Qca_LIBRARY
    NAMES qca-qt6
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )
endif ()

find_package_handle_standard_args(Qca REQUIRED_VARS Qca_LIBRARY Qca_INCLUDE_DIR)

if (Qca_FOUND AND NOT TARGET Qca::Qca)
  add_library(Qca::Qca STATIC IMPORTED)
  set_target_properties(
    Qca::Qca PROPERTIES IMPORTED_LOCATION "${Qca_LIBRARY}" INTERFACE_INCLUDE_DIRECTORIES
                                                           "${Qca_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Qca_LIBRARY Qca_INCLUDE_DIR)
