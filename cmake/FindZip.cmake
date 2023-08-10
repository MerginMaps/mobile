# GPLv2 Licence

  find_path(
    Zip_INCLUDE_DIR
    zip.h
    "${INPUT_SDK_PATH_MULTI}/include"
    NO_DEFAULT_PATH
  )

  find_library(
    Zip_LIBRARY
    NAMES zip
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )

find_package_handle_standard_args(Zip REQUIRED_VARS Zip_LIBRARY Zip_INCLUDE_DIR)

if (Zip_FOUND AND NOT TARGET Zip::Zip)
  add_library(Zip::Zip UNKNOWN IMPORTED)
  set_target_properties(
    Zip::Zip PROPERTIES IMPORTED_LOCATION "${Zip_LIBRARY}" INTERFACE_INCLUDE_DIRECTORIES
                                                           "${Zip_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Zip_LIBRARY Zip_INCLUDE_DIR)
