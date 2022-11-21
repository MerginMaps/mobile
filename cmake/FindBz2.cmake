# GPLv2 Licence

# not in macos input-SDK not in linux input-SDK

if (LINUX OR MACOS)
  find_library(Bz2_LIBRARY NAMES bz2)
elseif (ANDROID)
  message(FATAL_ERROR "bz2 is not required on ANDROID")
else ()
  # TODO find_path(Bz2_INCLUDE_DIR tiff.h "${INPUT_SDK_PATH}/include" NO_DEFAULT_PATH )

  find_library(Bz2_LIBRARY NAMES bz2 # PATHS "${INPUT_SDK_PATH}/lib" NO_DEFAULT_PATH
  )
endif ()

find_package_handle_standard_args(Bz2 REQUIRED_VARS Bz2_LIBRARY # Bz2_INCLUDE_DIR
)

if (Bz2_FOUND AND NOT TARGET Bz2::Bz2)
  add_library(Bz2::Bz2 UNKNOWN IMPORTED)
  set_target_properties(
    Bz2::Bz2 PROPERTIES IMPORTED_LOCATION "${Bz2_LIBRARY}" # INTERFACE_INCLUDE_DIRECTORIES
                                                           # "${Bz2_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(
  Bz2_LIBRARY
  # Bz2_INCLUDE_DIR
)
