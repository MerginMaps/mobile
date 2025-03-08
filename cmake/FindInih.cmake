# GPLv2 Licence
find_library(
  Inih_LIBRARY
  NAMES libinih inih
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

find_library(
  Inireader_LIBRARY
  NAMES libINIReader INIReader
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(Inih REQUIRED_VARS Inih_LIBRARY Inireader_LIBRARY)

if (Inih_FOUND AND NOT TARGET Inih::Inih)
  add_library(Inih::Inih UNKNOWN IMPORTED)
  set_target_properties(Inih::Inih PROPERTIES IMPORTED_LOCATION "${Inih_LIBRARY}")

  add_library(Inih::Inireader UNKNOWN IMPORTED)
  set_target_properties(
    Inih::Inireader PROPERTIES IMPORTED_LOCATION "${Inireader_LIBRARY}"
  )
endif ()

mark_as_advanced(Inih_LIBRARY Inireader_LIBRARY)
