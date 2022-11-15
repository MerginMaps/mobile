# GPLv2 Licence

find_path(Qca_INCLUDE_DIR Qca-qt6/QtCrypto/qca.h
  "${INPUT_SDK_PATH}/include"
  NO_DEFAULT_PATH
)
  
find_library(Qca_LIBRARY 
  NAMES qca-qt6 
  PATHS "${INPUT_SDK_PATH}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(
  Qca
  REQUIRED_VARS Qca_LIBRARY Qca_INCLUDE_DIR
)

if(Qca_FOUND AND NOT TARGET Qca::Qca)
  add_library(Qca::Qca STATIC IMPORTED)
  set_target_properties(Qca::Qca PROPERTIES
    IMPORTED_LOCATION "${Qca_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Qca_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  Qca_LIBRARY
  Qca_INCLUDE_DIR
)