# GPLv2 Licence

# not in linux input-SDK

if (LINUX)
  find_path(Zip_INCLUDE_DIR NAMES zip.h)
  find_library(Zip_LIBRARY NAMES zip)
else ()
  find_path(
    Zip_INCLUDE_DIR
    zip.h
    "${INPUT_SDK_PATH}/include"
    NO_DEFAULT_PATH
  )

  find_library(
    Zip_LIBRARY
    NAMES zip
    PATHS "${INPUT_SDK_PATH}/lib"
    NO_DEFAULT_PATH
  )
endif ()

find_package_handle_standard_args(Zip REQUIRED_VARS Zip_LIBRARY Zip_INCLUDE_DIR)

if (Zip_FOUND AND NOT TARGET Zip::Zip)
  add_library(Zip::Zip UNKNOWN IMPORTED)
  set_target_properties(
    Zip::Zip PROPERTIES IMPORTED_LOCATION "${Zip_LIBRARY}" INTERFACE_INCLUDE_DIRECTORIES
                                                           "${Zip_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Zip_LIBRARY Zip_INCLUDE_DIR)
