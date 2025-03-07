# GPLv2 Licence

find_path(
  Protobuf_INCLUDE_DIR
  google/protobuf/service.h
  "${INPUT_SDK_PATH_MULTI}/include"
  NO_DEFAULT_PATH
)

find_library(
  Protobuf_lite_LIBRARY
  NAMES protobuf-lite libprotobuf-lite
  PATHS "${INPUT_SDK_PATH_MULTI}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(
  Protobuf REQUIRED_VARS Protobuf_lite_LIBRARY Protobuf_INCLUDE_DIR
)

if (Protobuf_FOUND AND NOT TARGET Protobuf::Protobuf-lite)
  add_library(Protobuf::Protobuf-lite UNKNOWN IMPORTED)
  set_target_properties(
    Protobuf::Protobuf-lite
    PROPERTIES IMPORTED_LOCATION "${Protobuf_lite_LIBRARY}" INTERFACE_INCLUDE_DIRECTORIES
                                                            "${Protobuf_INCLUDE_DIR}"
  )

endif ()

mark_as_advanced(Protobuf_lite_LIBRARY Protobuf_INCLUDE_DIR)
