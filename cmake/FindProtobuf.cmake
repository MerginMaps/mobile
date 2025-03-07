# GPLv2 Licence

find_package(absl CONFIG REQUIRED)

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

if (WIN)
  set(absl_deps
      "absl::absl_decode_rust_punycode;absl::absl_demangle_rust;absl::absl_flags_commandlineflag;absl::absl_flags_commandlineflag_internal;absl::absl_flags_config;absl::absl_flags_internal;absl::absl_flags_marshalling;absl::absl_flags_parse;absl::absl_flags_private_handle_accessor;absl::absl_flags_program_name;absl::absl_flags_reflection;absl::absl_flags_usage;absl::absl_flags_usage_internal;absl::absl_log_flags;absl::absl_log_internal_structured_proto;absl::absl_poison;absl::absl_tracing_internal;absl::absl_utf8_for_code_point"
  )
else ()
  set(absl_deps
      "absl::absl_check;absl::absl_log;absl::algorithm;absl::base;absl::bind_front;absl::bits;absl::btree;absl::cleanup;absl::cord;absl::core_headers;absl::debugging;absl::die_if_null;absl::dynamic_annotations;absl::flags;absl::flat_hash_map;absl::flat_hash_set;absl::function_ref;absl::hash;absl::if_constexpr;absl::layout;absl::log_initialize;absl::log_globals;absl::log_severity;absl::memory;absl::node_hash_map;absl::node_hash_set;absl::optional;absl::random_distributions;absl::random_random;absl::span;absl::status;absl::statusor;absl::strings;absl::synchronization;absl::time;absl::type_traits;absl::utility;absl::variant"
  )
endif ()

if (Protobuf_FOUND AND NOT TARGET Protobuf::Protobuf-lite)
  add_library(Protobuf::Protobuf-lite UNKNOWN IMPORTED)
  set_target_properties(
    Protobuf::Protobuf-lite
    PROPERTIES IMPORTED_LOCATION "${Protobuf_lite_LIBRARY}"
               INTERFACE_INCLUDE_DIRECTORIES "${Protobuf_INCLUDE_DIR}"
               INTERFACE_LINK_LIBRARIES "${absl_deps}"
  )

endif ()

mark_as_advanced(Protobuf_lite_LIBRARY Protobuf_INCLUDE_DIR)
