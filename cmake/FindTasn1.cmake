# GPLv2 Licence

# not in linux and win and android input-SDK

if (LNX
    OR WIN
    OR ANDROID
)
  message(FATAL_ERROR "tasn1 is not available for this platform in input-SDK")
else ()
  find_path(
    Tasn1_INCLUDE_DIR
    libtasn1.h
    "${INPUT_SDK_PATH_MULTI}/include"
    NO_DEFAULT_PATH
  )

  find_library(
    Tasn1_LIBRARY
    NAMES tasn1
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )
endif ()

find_package_handle_standard_args(Tasn1 REQUIRED_VARS Tasn1_LIBRARY Tasn1_INCLUDE_DIR)

if (Tasn1_FOUND AND NOT TARGET Tasn1::Tasn1)
  add_library(Tasn1::Tasn1 STATIC IMPORTED)
  set_target_properties(
    Tasn1::Tasn1 PROPERTIES IMPORTED_LOCATION "${Tasn1_LIBRARY}"
                            INTERFACE_INCLUDE_DIRECTORIES "${Tasn1_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Tasn1_LIBRARY Tasn1_INCLUDE_DIR)
