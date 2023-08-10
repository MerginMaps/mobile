# GPLv2 Licence

find_library(
  Xml2_LIBRARY
  NAMES libxml2
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(Xml2 REQUIRED_VARS Xml2_LIBRARY)

if (Xml2_FOUND AND NOT TARGET Xml2::Xml2)
  add_library(Xml2::Xml2 UNKNOWN IMPORTED)
  set_target_properties(Xml2::Xml2 PROPERTIES IMPORTED_LOCATION "${Xml2_LIBRARY}")
endif ()

mark_as_advanced(Xml2_LIBRARY)
