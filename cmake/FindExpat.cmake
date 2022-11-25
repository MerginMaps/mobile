# GPLv2 Licence

# not in linux input-SDK

if (LNX)
  find_path(Expat_INCLUDE_DIR NAMES expat.h)
  find_library(Expat_LIBRARY NAMES expat)
elseif (WIN)
  find_path(
    Expat_INCLUDE_DIR
    expat.h
    "${INPUT_SDK_PATH_MULTI}/include"
    NO_DEFAULT_PATH
  )

  find_library(
    Expat_LIBRARY
    NAMES libexpat
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )
else ()
  find_path(
    Expat_INCLUDE_DIR
    expat.h
    "${INPUT_SDK_PATH_MULTI}/include"
    NO_DEFAULT_PATH
  )

  find_library(
    Expat_LIBRARY
    NAMES expat
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )
endif ()

find_package_handle_standard_args(Expat REQUIRED_VARS Expat_LIBRARY Expat_INCLUDE_DIR)

if (Expat_FOUND AND NOT TARGET Expat::Expat)
  add_library(Expat::Expat UNKNOWN IMPORTED)
  set_target_properties(
    Expat::Expat PROPERTIES IMPORTED_LOCATION "${Expat_LIBRARY}"
                            INTERFACE_INCLUDE_DIRECTORIES "${Expat_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Expat_LIBRARY Expat_INCLUDE_DIR)
