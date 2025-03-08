# GPLv2 Licence

set(absl_libs
    abseil_dll
    algorithm
    algorithm_container
    any
    any_invocable
    atomic_hook
    bad_any_cast
    bad_any_cast_impl
    bad_optional_access
    bad_variant_access
    bind_front
    bits
    bounded_utf8_length_sequence
    btree
    charset
    check
    city
    civil_time
    cleanup
    cleanup_internal
    common_policy_traits
    compare
    compressed_tuple
    config
    container_common
    container_memory
    cord
    cord_internal
    cordz_functions
    cordz_handle
    cordz_info
    cordz_sample_token
    cordz_statistics
    cordz_update_scope
    cordz_update_tracker
    core_headers
    crc32c
    crc_cord_state
    crc_cpu_detect
    crc_internal
    debugging
    debugging_internal
    demangle_internal
    demangle_rust
    decode_rust_punycode
    die_if_null
    dynamic_annotations
    endian
    errno_saver
    examine_stack
    exponential_biased
    failure_signal_handler
    fast_type_id
    fixed_array
    flags
    flags_commandlineflag
    flags_commandlineflag_internal
    flags_config
    flags_config;flags_internal
    flags_internal
    flags_marshalling
    flags_marshalling;flags_parse
    flags_parse
    flags_path_util
    flags_private_handle_accessor
    flags_private_handle_accessor;flags_program_name
    flags_program_name
    flags_reflection
    flags_reflection;flags_usage
    flags_usage
    flags_usage_internal
    flat_hash_map
    flat_hash_set
    function_ref
    graphcycles_internal
    has_ostream_operator
    hash
    hash_container_defaults
    hash_function_defaults
    hash_policy_traits
    hashtable_debug
    hashtable_debug_hooks
    hashtablez_sampler
    if_constexpr
    inlined_vector
    inlined_vector_internal
    int128
    kernel_timeout_internal
    layout
    leak_check
    log
    log_entry
    log_flags
    log_flags;log_internal_structured_proto
    log_globals
    log_initialize
    log_internal_append_truncated
    log_internal_check_impl
    log_internal_check_op
    log_internal_conditions
    log_internal_config
    log_internal_flags
    log_internal_fnmatch
    log_internal_format
    log_internal_globals
    log_internal_log_impl
    log_internal_log_sink_set
    log_internal_message
    log_internal_nullguard
    log_internal_nullstream
    log_internal_proto
    log_internal_strip
    log_internal_structured
    log_internal_structured_proto
    log_internal_voidify
    log_severity
    log_sink
    log_sink_registry
    log_streamer
    log_structured
    low_level_hash
    malloc_internal
    memory
    meta
    no_destructor
    node_hash_map
    node_hash_set
    node_slot_policy
    non_temporal_arm_intrinsics
    non_temporal_memcpy
    nullability
    numeric
    numeric_representation
    optional
    overload
    periodic_sampler
    poison
    prefetch
    pretty_function
    random_bit_gen_ref
    random_distributions
    random_internal_distribution_caller
    random_internal_distribution_test_util
    random_internal_fast_uniform_bits
    random_internal_fastmath
    random_internal_generate_real
    random_internal_iostream_state_saver
    random_internal_mock_helpers
    random_internal_nonsecure_base
    random_internal_pcg_engine
    random_internal_platform
    random_internal_pool_urbg
    random_internal_randen
    random_internal_randen_engine
    random_internal_randen_hwaes
    random_internal_randen_hwaes_impl
    random_internal_randen_slow
    random_internal_salted_seed_seq
    random_internal_seed_material
    random_internal_traits
    random_internal_uniform_helper
    random_internal_wide_multiply
    random_random
    random_seed_gen_exception
    random_seed_sequences
    raw_hash_map
    raw_hash_set
    raw_logging_internal
    sample_recorder
    scoped_set_env
    span
    spinlock_wait
    stacktrace
    status
    statusor
    str_format
    str_format_internal
    strerror
    string_view
    strings
    strings_internal
    symbolize
    synchronization
    throw_delegate
    time
    time_zone
    tracing_internal
    type_traits
    utf8_for_code_point
    utility
    variant
    vlog_config_internal
    vlog_is_on
    base
    base_internal
)

foreach (absl_lib IN ITEMS ${absl_libs})

  find_library(
    absl_${absl_lib}_LIBRARY
    NAMES absl_${absl_lib} libabsl_${absl_lib} ${absl_lib}
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )
  if (NOT TARGET Absl::${absl_lib})
    if (EXISTS ${absl_${absl_lib}_LIBRARY})
      add_library(Absl::${absl_lib} UNKNOWN IMPORTED)
      set_target_properties(
        Absl::${absl_lib} PROPERTIES IMPORTED_LOCATION "${absl_${absl_lib}_LIBRARY}"
      )

      set(ABSL_TARGETS "${ABSL_TARGETS};Absl::${absl_lib}")
    else ()
      add_library(Absl::${absl_lib} INTERFACE IMPORTED)
    endif ()
  endif ()
endforeach ()


if (TARGET Absl::symbolize)
  set_target_properties(
    Absl::symbolize
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::debugging_internal;Absl::demangle_internal;Absl::base;Absl::config;Absl::core_headers;Absl::dynamic_annotations;Absl::malloc_internal;Absl::raw_logging_internal;Absl::strings;\$<LINK_ONLY:\$<\$<BOOL:>:-ldbghelp>>"
  )
endif ()

if (TARGET Absl::debugging_internal)
  set_target_properties(
    Absl::debugging_internal
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::core_headers;Absl::config;Absl::dynamic_annotations;Absl::errno_saver;Absl::raw_logging_internal"
  )
endif ()

find_package_handle_standard_args(absl REQUIRED_VARS ABSL_TARGETS)
