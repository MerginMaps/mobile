# GPLv2 Licence

set(absl_libs
    abseil_dll
    absl_vlog_is_on
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
  if (NOT TARGET "Absl::${absl_lib}")
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

if (TARGET Absl::atomic_hook)

  set_target_properties(
    Absl::atomic_hook PROPERTIES INTERFACE_LINK_LIBRARIES
                                 "Absl::config;Absl::core_headers"
  )

endif () # imported target Absl::errno_saver

if (TARGET Absl::errno_saver)

  set_target_properties(
    Absl::errno_saver PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config"
  )

endif () # imported target Absl::log_severity
if (TARGET Absl::log_severity)

  set_target_properties(
    Absl::log_severity PROPERTIES INTERFACE_LINK_LIBRARIES
                                  "Absl::config;Absl::core_headers"
  )

endif () # imported target Absl::no_destructor
if (TARGET Absl::no_destructor)

  set_target_properties(
    Absl::no_destructor PROPERTIES INTERFACE_LINK_LIBRARIES
                                   "Absl::config;Absl::nullability"
  )

endif () # imported target Absl::nullability
if (TARGET Absl::nullability)

  set_target_properties(
    Absl::nullability PROPERTIES INTERFACE_LINK_LIBRARIES
                                 "Absl::config;Absl::core_headers;Absl::type_traits"
  )

endif () # imported target Absl::raw_logging_internal
if (TARGET Absl::raw_logging_internal)

  set_target_properties(
    Absl::raw_logging_internal
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::atomic_hook;Absl::config;Absl::core_headers;Absl::errno_saver;Absl::log_severity"
  )

endif () # imported target Absl::spinlock_wait
if (TARGET Absl::spinlock_wait)

  set_target_properties(
    Absl::spinlock_wait
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::base_internal;Absl::core_headers;Absl::errno_saver"
  )

endif () # imported target Absl::config

if (TARGET Absl::dynamic_annotations)

  set_target_properties(
    Absl::dynamic_annotations PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config"
  )

endif () # imported target Absl::core_headers
if (TARGET Absl::core_headers)

  set_target_properties(
    Absl::core_headers PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config"
  )

endif () # imported target Absl::malloc_internal
if (TARGET Absl::malloc_internal)

  set_target_properties(
    Absl::malloc_internal
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base;Absl::base_internal;Absl::config;Absl::core_headers;Absl::dynamic_annotations;Absl::raw_logging_internal;Threads::Threads"
  )

endif () # imported target Absl::base_internal
if (TARGET Absl::base_internal)

  set_target_properties(
    Absl::base_internal PROPERTIES INTERFACE_LINK_LIBRARIES
                                   "Absl::config;Absl::type_traits"
  )

endif () # imported target Absl::base
if (TARGET Absl::base)

  set_target_properties(
    Absl::base
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::atomic_hook;Absl::base_internal;Absl::config;Absl::core_headers;Absl::dynamic_annotations;Absl::log_severity;Absl::nullability;Absl::raw_logging_internal;Absl::spinlock_wait;Absl::type_traits;Threads::Threads;\$<LINK_ONLY:\$<\$<BOOL:LIBRT-NOTFOUND>:-lrt>>;\$<LINK_ONLY:\$<\$<BOOL:>:-ladvapi32>>"
  )

endif () # imported target Absl::throw_delegate
if (TARGET Absl::throw_delegate)

  set_target_properties(
    Absl::throw_delegate PROPERTIES INTERFACE_LINK_LIBRARIES
                                    "Absl::config;Absl::raw_logging_internal"
  )

endif () # imported target Absl::pretty_function

if (TARGET Absl::endian)

  set_target_properties(
    Absl::endian
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::base;Absl::config;Absl::core_headers;Absl::nullability"
  )

endif () # imported target Absl::scoped_set_env
if (TARGET Absl::scoped_set_env)

  set_target_properties(
    Absl::scoped_set_env PROPERTIES INTERFACE_LINK_LIBRARIES
                                    "Absl::config;Absl::raw_logging_internal"
  )

endif () # imported target Absl::strerror
if (TARGET Absl::strerror)

  set_target_properties(
    Absl::strerror PROPERTIES INTERFACE_LINK_LIBRARIES
                              "Absl::config;Absl::core_headers;Absl::errno_saver"
  )

endif () # imported target Absl::fast_type_id
if (TARGET Absl::fast_type_id)

  set_target_properties(
    Absl::fast_type_id PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config"
  )

endif () # imported target Absl::prefetch
if (TARGET Absl::prefetch)

  set_target_properties(
    Absl::prefetch PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config;Absl::core_headers"
  )

endif () # imported target Absl::poison
if (TARGET Absl::poison)

  set_target_properties(
    Absl::poison PROPERTIES INTERFACE_LINK_LIBRARIES
                            "Absl::config;Absl::core_headers;Absl::malloc_internal"
  )

endif () # imported target Absl::tracing_internal
if (TARGET Absl::tracing_internal)

  set_target_properties(
    Absl::tracing_internal PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::base"
  )

endif () # imported target Absl::algorithm
if (TARGET Absl::algorithm)

  set_target_properties(
    Absl::algorithm PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config"
  )

endif () # imported target Absl::algorithm_container
if (TARGET Absl::algorithm_container)

  set_target_properties(
    Absl::algorithm_container
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::algorithm;Absl::config;Absl::core_headers;Absl::meta;Absl::nullability"
  )

endif () # imported target Absl::cleanup_internal
if (TARGET Absl::cleanup_internal)

  set_target_properties(
    Absl::cleanup_internal
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::base_internal;Absl::core_headers;Absl::utility"
  )

endif () # imported target Absl::cleanup
if (TARGET Absl::cleanup)

  set_target_properties(
    Absl::cleanup PROPERTIES INTERFACE_LINK_LIBRARIES
                             "Absl::cleanup_internal;Absl::config;Absl::core_headers"
  )

endif () # imported target Absl::btree
if (TARGET Absl::btree)

  set_target_properties(
    Absl::btree
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::common_policy_traits;Absl::compare;Absl::compressed_tuple;Absl::config;Absl::container_common;Absl::container_memory;Absl::cord;Absl::core_headers;Absl::layout;Absl::memory;Absl::raw_logging_internal;Absl::strings;Absl::throw_delegate;Absl::type_traits"
  )

endif () # imported target Absl::compressed_tuple
if (TARGET Absl::compressed_tuple)

  set_target_properties(
    Absl::compressed_tuple PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::utility"
  )

endif () # imported target Absl::fixed_array
if (TARGET Absl::fixed_array)

  set_target_properties(
    Absl::fixed_array
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::compressed_tuple;Absl::algorithm;Absl::config;Absl::core_headers;Absl::dynamic_annotations;Absl::throw_delegate;Absl::memory"
  )

endif () # imported target Absl::inlined_vector_internal
if (TARGET Absl::inlined_vector_internal)

  set_target_properties(
    Absl::inlined_vector_internal
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base_internal;Absl::compressed_tuple;Absl::config;Absl::core_headers;Absl::memory;Absl::span;Absl::type_traits"
  )

endif () # imported target Absl::inlined_vector
if (TARGET Absl::inlined_vector)

  set_target_properties(
    Absl::inlined_vector
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::algorithm;Absl::core_headers;Absl::inlined_vector_internal;Absl::throw_delegate;Absl::memory;Absl::type_traits"
  )

endif () # imported target Absl::flat_hash_map
if (TARGET Absl::flat_hash_map)

  set_target_properties(
    Absl::flat_hash_map
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::container_memory;Absl::core_headers;Absl::hash_container_defaults;Absl::raw_hash_map;Absl::algorithm_container;Absl::type_traits"
  )

endif () # imported target Absl::flat_hash_set
if (TARGET Absl::flat_hash_set)

  set_target_properties(
    Absl::flat_hash_set
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::container_memory;Absl::hash_container_defaults;Absl::raw_hash_set;Absl::algorithm_container;Absl::core_headers;Absl::memory;Absl::type_traits"
  )

endif () # imported target Absl::node_hash_map
if (TARGET Absl::node_hash_map)

  set_target_properties(
    Absl::node_hash_map
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::container_memory;Absl::core_headers;Absl::hash_container_defaults;Absl::node_slot_policy;Absl::raw_hash_map;Absl::algorithm_container;Absl::memory;Absl::type_traits"
  )

endif () # imported target Absl::node_hash_set
if (TARGET Absl::node_hash_set)

  set_target_properties(
    Absl::node_hash_set
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::container_memory;Absl::core_headers;Absl::hash_container_defaults;Absl::node_slot_policy;Absl::raw_hash_set;Absl::algorithm_container;Absl::memory;Absl::type_traits"
  )

endif () # imported target Absl::hash_container_defaults
if (TARGET Absl::hash_container_defaults)

  set_target_properties(
    Absl::hash_container_defaults PROPERTIES INTERFACE_LINK_LIBRARIES
                                             "Absl::config;Absl::hash_function_defaults"
  )

endif () # imported target Absl::container_memory
if (TARGET Absl::container_memory)

  set_target_properties(
    Absl::container_memory
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::config;Absl::memory;Absl::type_traits;Absl::utility"
  )

endif () # imported target Absl::hash_function_defaults
if (TARGET Absl::hash_function_defaults)

  set_target_properties(
    Absl::hash_function_defaults
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::container_common;Absl::cord;Absl::hash;Absl::strings;Absl::type_traits"
  )

endif () # imported target Absl::hash_policy_traits
if (TARGET Absl::hash_policy_traits)

  set_target_properties(
    Absl::hash_policy_traits PROPERTIES INTERFACE_LINK_LIBRARIES
                                        "Absl::common_policy_traits;Absl::meta"
  )

endif () # imported target Absl::common_policy_traits
if (TARGET Absl::common_policy_traits)

  set_target_properties(
    Absl::common_policy_traits PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::meta"
  )

endif () # imported target Absl::hashtablez_sampler
if (TARGET Absl::hashtablez_sampler)

  set_target_properties(
    Absl::hashtablez_sampler
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base;Absl::config;Absl::exponential_biased;Absl::no_destructor;Absl::raw_logging_internal;Absl::sample_recorder;Absl::synchronization;Absl::time"
  )

endif () # imported target Absl::hashtable_debug
if (TARGET Absl::hashtable_debug)

  set_target_properties(
    Absl::hashtable_debug PROPERTIES INTERFACE_LINK_LIBRARIES
                                     "Absl::hashtable_debug_hooks"
  )

endif () # imported target Absl::hashtable_debug_hooks
if (TARGET Absl::hashtable_debug_hooks)

  set_target_properties(
    Absl::hashtable_debug_hooks PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config"
  )

endif () # imported target Absl::node_slot_policy
if (TARGET Absl::node_slot_policy)

  set_target_properties(
    Absl::node_slot_policy PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config"
  )

endif () # imported target Absl::raw_hash_map
if (TARGET Absl::raw_hash_map)

  set_target_properties(
    Absl::raw_hash_map
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::container_memory;Absl::core_headers;Absl::raw_hash_set;Absl::throw_delegate"
  )

endif () # imported target Absl::container_common
if (TARGET Absl::container_common)

  set_target_properties(
    Absl::container_common PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::type_traits"
  )

endif () # imported target Absl::raw_hash_set
if (TARGET Absl::raw_hash_set)

  set_target_properties(
    Absl::raw_hash_set
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::bits;Absl::common_policy_traits;Absl::compressed_tuple;Absl::config;Absl::container_common;Absl::container_memory;Absl::core_headers;Absl::dynamic_annotations;Absl::endian;Absl::hash;Absl::hash_function_defaults;Absl::hash_policy_traits;Absl::hashtable_debug_hooks;Absl::hashtablez_sampler;Absl::memory;Absl::meta;Absl::optional;Absl::prefetch;Absl::raw_logging_internal;Absl::utility"
  )

endif () # imported target Absl::layout
if (TARGET Absl::layout)

  set_target_properties(
    Absl::layout
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::core_headers;Absl::debugging_internal;Absl::meta;Absl::strings;Absl::span;Absl::utility"
  )

endif () # imported target Absl::crc_cpu_detect
if (TARGET Absl::crc_cpu_detect)

  set_target_properties(
    Absl::crc_cpu_detect PROPERTIES INTERFACE_LINK_LIBRARIES
                                    "Absl::base;Absl::config;Absl::optional"
  )

endif () # imported target Absl::crc_internal
if (TARGET Absl::crc_internal)

  set_target_properties(
    Absl::crc_internal
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::crc_cpu_detect;Absl::config;Absl::core_headers;Absl::endian;Absl::prefetch;Absl::raw_logging_internal;Absl::memory;Absl::bits"
  )

endif () # imported target Absl::crc32c
if (TARGET Absl::crc32c)

  set_target_properties(
    Absl::crc32c
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::crc_cpu_detect;Absl::crc_internal;Absl::non_temporal_memcpy;Absl::config;Absl::core_headers;Absl::endian;Absl::prefetch;Absl::str_format;Absl::strings"
  )

endif () # imported target Absl::non_temporal_arm_intrinsics
if (TARGET Absl::non_temporal_arm_intrinsics)

  set_target_properties(
    Absl::non_temporal_arm_intrinsics PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config"
  )

endif () # imported target Absl::non_temporal_memcpy
if (TARGET Absl::non_temporal_memcpy)

  set_target_properties(
    Absl::non_temporal_memcpy
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::non_temporal_arm_intrinsics;Absl::config;Absl::core_headers"
  )

endif () # imported target Absl::crc_cord_state
if (TARGET Absl::crc_cord_state)

  set_target_properties(
    Absl::crc_cord_state
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::crc32c;Absl::config;Absl::strings;Absl::no_destructor"
  )

endif () # imported target Absl::stacktrace
if (TARGET Absl::stacktrace)

  set_target_properties(
    Absl::stacktrace
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::debugging_internal;Absl::config;Absl::core_headers;Absl::dynamic_annotations;Absl::raw_logging_internal;\$<LINK_ONLY:\$<\$<BOOL:EXECINFO_LIBRARY-NOTFOUND>:EXECINFO_LIBRARY-NOTFOUND>>"
  )

endif () # imported target Absl::symbolize
if (TARGET Absl::symbolize)

  set_target_properties(
    Absl::symbolize
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::debugging_internal;Absl::demangle_internal;Absl::base;Absl::config;Absl::core_headers;Absl::dynamic_annotations;Absl::malloc_internal;Absl::raw_logging_internal;Absl::strings;\$<LINK_ONLY:\$<\$<BOOL:>:-ldbghelp>>"
  )

endif () # imported target Absl::examine_stack
if (TARGET Absl::examine_stack)

  set_target_properties(
    Absl::examine_stack
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::stacktrace;Absl::symbolize;Absl::config;Absl::core_headers;Absl::raw_logging_internal"
  )

endif () # imported target Absl::failure_signal_handler
if (TARGET Absl::failure_signal_handler)

  set_target_properties(
    Absl::failure_signal_handler
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::examine_stack;Absl::stacktrace;Absl::base;Absl::config;Absl::core_headers;Absl::raw_logging_internal"
  )

endif () # imported target Absl::debugging_internal
if (TARGET Absl::debugging_internal)

  set_target_properties(
    Absl::debugging_internal
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::core_headers;Absl::config;Absl::dynamic_annotations;Absl::errno_saver;Absl::raw_logging_internal"
  )

endif () # imported target Absl::demangle_internal
if (TARGET Absl::demangle_internal)

  set_target_properties(
    Absl::demangle_internal PROPERTIES INTERFACE_LINK_LIBRARIES
                                       "Absl::config;Absl::demangle_rust"
  )

endif () # imported target Absl::bounded_utf8_length_sequence
if (TARGET Absl::bounded_utf8_length_sequence)

  set_target_properties(
    Absl::bounded_utf8_length_sequence PROPERTIES INTERFACE_LINK_LIBRARIES
                                                  "Absl::bits;Absl::config"
  )

endif () # imported target Absl::decode_rust_punycode
if (TARGET Absl::decode_rust_punycode)

  set_target_properties(
    Absl::decode_rust_punycode
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::bounded_utf8_length_sequence;Absl::config;Absl::nullability;Absl::utf8_for_code_point"
  )

endif () # imported target Absl::demangle_rust
if (TARGET Absl::demangle_rust)

  set_target_properties(
    Absl::demangle_rust
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::config;Absl::core_headers;Absl::decode_rust_punycode"
  )

endif () # imported target Absl::utf8_for_code_point
if (TARGET Absl::utf8_for_code_point)

  set_target_properties(
    Absl::utf8_for_code_point PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config"
  )

endif () # imported target Absl::leak_check
if (TARGET Absl::leak_check)

  set_target_properties(
    Absl::leak_check PROPERTIES INTERFACE_LINK_LIBRARIES
                                "Absl::config;Absl::core_headers"
  )

endif () # imported target Absl::debugging
if (TARGET Absl::debugging)

  set_target_properties(
    Absl::debugging PROPERTIES INTERFACE_LINK_LIBRARIES
                               "Absl::stacktrace;Absl::leak_check"
  )

endif () # imported target Absl::flags_path_util
if (TARGET Absl::flags_path_util)

  set_target_properties(
    Absl::flags_path_util PROPERTIES INTERFACE_LINK_LIBRARIES
                                     "Absl::config;Absl::strings"
  )

endif () # imported target Absl::flags_program_name
if (TARGET Absl::flags_program_name)

  set_target_properties(
    Absl::flags_program_name
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::core_headers;Absl::no_destructor;Absl::flags_path_util;Absl::strings;Absl::synchronization"
  )

endif () # imported target Absl::flags_config
if (TARGET Absl::flags_config)

  set_target_properties(
    Absl::flags_config
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::flags_path_util;Absl::flags_program_name;Absl::core_headers;Absl::no_destructor;Absl::strings;Absl::synchronization"
  )

endif () # imported target Absl::flags_marshalling
if (TARGET Absl::flags_marshalling)

  set_target_properties(
    Absl::flags_marshalling
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::core_headers;Absl::log_severity;Absl::int128;Absl::optional;Absl::strings;Absl::str_format"
  )

endif () # imported target Absl::flags_commandlineflag_internal
if (TARGET Absl::flags_commandlineflag_internal)

  set_target_properties(
    Absl::flags_commandlineflag_internal
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::config;Absl::dynamic_annotations;Absl::fast_type_id"
  )

endif () # imported target Absl::flags_commandlineflag
if (TARGET Absl::flags_commandlineflag)

  set_target_properties(
    Absl::flags_commandlineflag
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::fast_type_id;Absl::flags_commandlineflag_internal;Absl::optional;Absl::strings"
  )

endif () # imported target Absl::flags_private_handle_accessor
if (TARGET Absl::flags_private_handle_accessor)

  set_target_properties(
    Absl::flags_private_handle_accessor
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::flags_commandlineflag;Absl::flags_commandlineflag_internal;Absl::strings"
  )

endif () # imported target Absl::flags_reflection
if (TARGET Absl::flags_reflection)

  set_target_properties(
    Absl::flags_reflection
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::flags_commandlineflag;Absl::flags_private_handle_accessor;Absl::flags_config;Absl::strings;Absl::synchronization;Absl::flat_hash_map;Absl::no_destructor"
  )

endif () # imported target Absl::flags_internal
if (TARGET Absl::flags_internal)

  set_target_properties(
    Absl::flags_internal
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base;Absl::config;Absl::flags_commandlineflag;Absl::flags_commandlineflag_internal;Absl::flags_config;Absl::flags_marshalling;Absl::no_destructor;Absl::synchronization;Absl::meta;Absl::utility"
  )

endif () # imported target Absl::flags
if (TARGET Absl::flags)

  set_target_properties(
    Absl::flags
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::flags_commandlineflag;Absl::flags_config;Absl::flags_internal;Absl::flags_reflection;Absl::core_headers;Absl::strings"
  )

endif () # imported target Absl::flags_usage_internal
if (TARGET Absl::flags_usage_internal)

  set_target_properties(
    Absl::flags_usage_internal
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::flags_config;Absl::flags;Absl::flags_commandlineflag;Absl::flags_internal;Absl::flags_path_util;Absl::flags_private_handle_accessor;Absl::flags_program_name;Absl::flags_reflection;Absl::strings;Absl::synchronization"
  )

endif () # imported target Absl::flags_usage
if (TARGET Absl::flags_usage)

  set_target_properties(
    Absl::flags_usage
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::core_headers;Absl::flags_usage_internal;Absl::no_destructor;Absl::raw_logging_internal;Absl::strings;Absl::synchronization"
  )

endif () # imported target Absl::flags_parse
if (TARGET Absl::flags_parse)

  set_target_properties(
    Absl::flags_parse
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::algorithm_container;Absl::config;Absl::core_headers;Absl::flags_config;Absl::flags;Absl::flags_commandlineflag;Absl::flags_commandlineflag_internal;Absl::flags_internal;Absl::flags_private_handle_accessor;Absl::flags_program_name;Absl::flags_reflection;Absl::flags_usage;Absl::no_destructor;Absl::strings;Absl::synchronization"
  )

endif () # imported target Absl::any_invocable
if (TARGET Absl::any_invocable)

  set_target_properties(
    Absl::any_invocable
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base_internal;Absl::config;Absl::core_headers;Absl::type_traits;Absl::utility"
  )

endif () # imported target Absl::bind_front
if (TARGET Absl::bind_front)

  set_target_properties(
    Absl::bind_front PROPERTIES INTERFACE_LINK_LIBRARIES
                                "Absl::base_internal;Absl::compressed_tuple"
  )

endif () # imported target Absl::function_ref
if (TARGET Absl::function_ref)

  set_target_properties(
    Absl::function_ref
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::base_internal;Absl::core_headers;Absl::any_invocable;Absl::meta"
  )

endif () # imported target Absl::overload
if (TARGET Absl::overload)

  set_target_properties(Absl::overload PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::meta")

endif () # imported target Absl::hash
if (TARGET Absl::hash)

  set_target_properties(
    Absl::hash
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::bits;Absl::city;Absl::config;Absl::core_headers;Absl::endian;Absl::fixed_array;Absl::function_ref;Absl::meta;Absl::int128;Absl::strings;Absl::optional;Absl::variant;Absl::utility;Absl::low_level_hash"
  )

endif () # imported target Absl::city
if (TARGET Absl::city)

  set_target_properties(
    Absl::city PROPERTIES INTERFACE_LINK_LIBRARIES
                          "Absl::config;Absl::core_headers;Absl::endian"
  )

endif () # imported target Absl::low_level_hash
if (TARGET Absl::low_level_hash)

  set_target_properties(
    Absl::low_level_hash
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::config;Absl::endian;Absl::int128;Absl::prefetch"
  )

endif () # imported target Absl::log_internal_check_impl
if (TARGET Absl::log_internal_check_impl)

  set_target_properties(
    Absl::log_internal_check_impl
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::core_headers;Absl::log_internal_check_op;Absl::log_internal_conditions;Absl::log_internal_message;Absl::log_internal_strip"
  )

endif () # imported target Absl::log_internal_check_op
if (TARGET Absl::log_internal_check_op)

  set_target_properties(
    Absl::log_internal_check_op
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base;Absl::config;Absl::core_headers;Absl::leak_check;Absl::log_internal_nullguard;Absl::log_internal_nullstream;Absl::log_internal_strip;Absl::nullability;Absl::strings"
  )

endif () # imported target Absl::log_internal_conditions
if (TARGET Absl::log_internal_conditions)

  set_target_properties(
    Absl::log_internal_conditions
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::base;Absl::config;Absl::core_headers;Absl::log_internal_voidify"
  )

endif () # imported target Absl::log_internal_config
if (TARGET Absl::log_internal_config)

  set_target_properties(
    Absl::log_internal_config PROPERTIES INTERFACE_LINK_LIBRARIES
                                         "Absl::config;Absl::core_headers"
  )

endif () # imported target Absl::log_internal_flags
if (TARGET Absl::log_internal_flags)

  set_target_properties(
    Absl::log_internal_flags PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::flags"
  )

endif () # imported target Absl::log_internal_format
if (TARGET Absl::log_internal_format)

  set_target_properties(
    Absl::log_internal_format
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::core_headers;Absl::log_internal_append_truncated;Absl::log_internal_config;Absl::log_internal_globals;Absl::log_severity;Absl::strings;Absl::str_format;Absl::time;Absl::span"
  )

endif () # imported target Absl::log_internal_globals
if (TARGET Absl::log_internal_globals)

  set_target_properties(
    Absl::log_internal_globals
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::core_headers;Absl::log_severity;Absl::raw_logging_internal;Absl::strings;Absl::time"
  )

endif () # imported target Absl::log_internal_log_impl
if (TARGET Absl::log_internal_log_impl)

  set_target_properties(
    Absl::log_internal_log_impl
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::log_internal_conditions;Absl::log_internal_message;Absl::log_internal_strip;Absl::absl_vlog_is_on"
  )

endif () # imported target Absl::log_internal_proto
if (TARGET Absl::log_internal_proto)

  set_target_properties(
    Absl::log_internal_proto
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::base;Absl::config;Absl::core_headers;Absl::strings;Absl::span"
  )

endif () # imported target Absl::log_internal_message
if (TARGET Absl::log_internal_message)

  set_target_properties(
    Absl::log_internal_message
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base;Absl::config;Absl::core_headers;Absl::errno_saver;Absl::examine_stack;Absl::inlined_vector;Absl::log_internal_append_truncated;Absl::log_internal_format;Absl::log_internal_globals;Absl::log_internal_proto;Absl::log_internal_log_sink_set;Absl::log_internal_nullguard;Absl::log_internal_structured_proto;Absl::log_globals;Absl::log_entry;Absl::log_severity;Absl::log_sink;Absl::log_sink_registry;Absl::memory;Absl::nullability;Absl::raw_logging_internal;Absl::span;Absl::strerror;Absl::strings;Absl::time"
  )

endif () # imported target Absl::log_internal_log_sink_set
if (TARGET Absl::log_internal_log_sink_set)

  set_target_properties(
    Absl::log_internal_log_sink_set
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base;Absl::cleanup;Absl::config;Absl::core_headers;Absl::log_internal_config;Absl::log_internal_globals;Absl::log_globals;Absl::log_entry;Absl::log_severity;Absl::log_sink;Absl::no_destructor;Absl::raw_logging_internal;Absl::synchronization;Absl::span;Absl::strings;\$<LINK_ONLY:\$<\$<BOOL:>:-llog>>"
  )

endif () # imported target Absl::log_internal_nullguard
if (TARGET Absl::log_internal_nullguard)

  set_target_properties(
    Absl::log_internal_nullguard PROPERTIES INTERFACE_LINK_LIBRARIES
                                            "Absl::config;Absl::core_headers"
  )

endif () # imported target Absl::log_internal_nullstream
if (TARGET Absl::log_internal_nullstream)

  set_target_properties(
    Absl::log_internal_nullstream
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::config;Absl::core_headers;Absl::log_severity;Absl::strings"
  )

endif () # imported target Absl::log_internal_strip
if (TARGET Absl::log_internal_strip)

  set_target_properties(
    Absl::log_internal_strip
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::core_headers;Absl::log_internal_message;Absl::log_internal_nullstream;Absl::log_severity"
  )

endif () # imported target Absl::log_internal_voidify
if (TARGET Absl::log_internal_voidify)

  set_target_properties(
    Absl::log_internal_voidify PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config"
  )

endif () # imported target Absl::log_internal_append_truncated
if (TARGET Absl::log_internal_append_truncated)

  set_target_properties(
    Absl::log_internal_append_truncated
    PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config;Absl::strings;Absl::span"
  )

endif () # imported target Absl::absl_check
if (TARGET Absl::absl_check)

  set_target_properties(
    Absl::absl_check PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::log_internal_check_impl"
  )

endif () # imported target Absl::absl_log
if (TARGET Absl::absl_log)

  set_target_properties(
    Absl::absl_log PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::log_internal_log_impl"
  )

endif () # imported target Absl::check
if (TARGET Absl::check)

  set_target_properties(
    Absl::check
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::log_internal_check_impl;Absl::core_headers;Absl::log_internal_check_op;Absl::log_internal_conditions;Absl::log_internal_message;Absl::log_internal_strip"
  )

endif () # imported target Absl::die_if_null
if (TARGET Absl::die_if_null)

  set_target_properties(
    Absl::die_if_null
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::config;Absl::core_headers;Absl::log;Absl::strings"
  )

endif () # imported target Absl::log_flags
if (TARGET Absl::log_flags)

  set_target_properties(
    Absl::log_flags
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::core_headers;Absl::log_globals;Absl::log_severity;Absl::log_internal_config;Absl::log_internal_flags;Absl::flags;Absl::flags_marshalling;Absl::strings;Absl::vlog_config_internal"
  )

endif () # imported target Absl::log_globals
if (TARGET Absl::log_globals)

  set_target_properties(
    Absl::log_globals
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::atomic_hook;Absl::config;Absl::core_headers;Absl::hash;Absl::log_severity;Absl::raw_logging_internal;Absl::strings;Absl::vlog_config_internal"
  )

endif () # imported target Absl::log_initialize
if (TARGET Absl::log_initialize)

  set_target_properties(
    Absl::log_initialize
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::config;Absl::log_globals;Absl::log_internal_globals;Absl::time"
  )

endif () # imported target Absl::log
if (TARGET Absl::log)

  set_target_properties(
    Absl::log PROPERTIES INTERFACE_LINK_LIBRARIES
                         "Absl::log_internal_log_impl;Absl::vlog_is_on"
  )

endif () # imported target Absl::log_entry
if (TARGET Absl::log_entry)

  set_target_properties(
    Absl::log_entry
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::core_headers;Absl::log_internal_config;Absl::log_severity;Absl::span;Absl::strings;Absl::time"
  )

endif () # imported target Absl::log_sink
if (TARGET Absl::log_sink)

  set_target_properties(
    Absl::log_sink PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config;Absl::log_entry"
  )

endif () # imported target Absl::log_sink_registry
if (TARGET Absl::log_sink_registry)

  set_target_properties(
    Absl::log_sink_registry
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::log_sink;Absl::log_internal_log_sink_set;Absl::nullability"
  )

endif () # imported target Absl::log_streamer
if (TARGET Absl::log_streamer)

  set_target_properties(
    Absl::log_streamer
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::absl_log;Absl::log_severity;Absl::optional;Absl::strings;Absl::strings_internal;Absl::utility"
  )

endif () # imported target Absl::log_internal_structured
if (TARGET Absl::log_internal_structured)

  set_target_properties(
    Absl::log_internal_structured
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::any_invocable;Absl::config;Absl::core_headers;Absl::log_internal_message;Absl::log_internal_structured_proto;Absl::strings"
  )

endif () # imported target Absl::log_internal_structured_proto
if (TARGET Absl::log_internal_structured_proto)

  set_target_properties(
    Absl::log_internal_structured_proto
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::log_internal_proto;Absl::config;Absl::span;Absl::strings;Absl::variant"
  )

endif () # imported target Absl::log_structured
if (TARGET Absl::log_structured)

  set_target_properties(
    Absl::log_structured
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::core_headers;Absl::log_internal_structured;Absl::strings"
  )

endif () # imported target Absl::vlog_config_internal
if (TARGET Absl::vlog_config_internal)

  set_target_properties(
    Absl::vlog_config_internal
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base;Absl::config;Absl::core_headers;Absl::log_internal_fnmatch;Absl::memory;Absl::no_destructor;Absl::strings;Absl::synchronization;Absl::optional"
  )

endif () # imported target Absl::absl_vlog_is_on
if (TARGET Absl::absl_vlog_is_on)

  set_target_properties(
    Absl::absl_vlog_is_on
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::vlog_config_internal;Absl::config;Absl::core_headers;Absl::strings"
  )

endif () # imported target Absl::vlog_is_on
if (TARGET Absl::vlog_is_on)

  set_target_properties(
    Absl::vlog_is_on PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::absl_vlog_is_on"
  )

endif () # imported target Absl::log_internal_fnmatch
if (TARGET Absl::log_internal_fnmatch)

  set_target_properties(
    Absl::log_internal_fnmatch PROPERTIES INTERFACE_LINK_LIBRARIES
                                          "Absl::config;Absl::strings"
  )

endif () # imported target Absl::memory
if (TARGET Absl::memory)

  set_target_properties(
    Absl::memory PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::core_headers;Absl::meta"
  )

endif () # imported target Absl::type_traits
if (TARGET Absl::type_traits)

  set_target_properties(
    Absl::type_traits PROPERTIES INTERFACE_LINK_LIBRARIES
                                 "Absl::config;Absl::core_headers"
  )

endif () # imported target Absl::meta
if (TARGET Absl::meta)

  set_target_properties(
    Absl::meta PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::type_traits"
  )

endif () # imported target Absl::bits
if (TARGET Absl::bits)

  set_target_properties(
    Absl::bits PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::core_headers"
  )

endif () # imported target Absl::int128
if (TARGET Absl::int128)

  set_target_properties(
    Absl::int128 PROPERTIES INTERFACE_LINK_LIBRARIES
                            "Absl::compare;Absl::config;Absl::core_headers;Absl::bits"
  )

endif () # imported target Absl::numeric
if (TARGET Absl::numeric)

  set_target_properties(Absl::numeric PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::int128")

endif () # imported target Absl::numeric_representation
if (TARGET Absl::numeric_representation)

  set_target_properties(
    Absl::numeric_representation PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config"
  )

endif () # imported target Absl::sample_recorder
if (TARGET Absl::sample_recorder)

  set_target_properties(
    Absl::sample_recorder PROPERTIES INTERFACE_LINK_LIBRARIES
                                     "Absl::base;Absl::synchronization"
  )

endif () # imported target Absl::exponential_biased
if (TARGET Absl::exponential_biased)

  set_target_properties(
    Absl::exponential_biased PROPERTIES INTERFACE_LINK_LIBRARIES
                                        "Absl::config;Absl::core_headers"
  )

endif () # imported target Absl::periodic_sampler
if (TARGET Absl::periodic_sampler)

  set_target_properties(
    Absl::periodic_sampler PROPERTIES INTERFACE_LINK_LIBRARIES
                                      "Absl::core_headers;Absl::exponential_biased"
  )

endif () # imported target Absl::random_random
if (TARGET Absl::random_random)

  set_target_properties(
    Absl::random_random
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::random_distributions;Absl::random_internal_nonsecure_base;Absl::random_internal_pcg_engine;Absl::random_internal_pool_urbg;Absl::random_internal_randen_engine;Absl::random_seed_sequences"
  )

endif () # imported target Absl::random_bit_gen_ref
if (TARGET Absl::random_bit_gen_ref)

  set_target_properties(
    Absl::random_bit_gen_ref
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::core_headers;Absl::random_internal_distribution_caller;Absl::random_internal_fast_uniform_bits;Absl::type_traits"
  )

endif () # imported target Absl::random_internal_mock_helpers
if (TARGET Absl::random_internal_mock_helpers)

  set_target_properties(
    Absl::random_internal_mock_helpers
    PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config;Absl::fast_type_id;Absl::optional"
  )

endif () # imported target Absl::random_distributions
if (TARGET Absl::random_distributions)

  set_target_properties(
    Absl::random_distributions
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base_internal;Absl::config;Absl::core_headers;Absl::random_internal_generate_real;Absl::random_internal_distribution_caller;Absl::random_internal_fast_uniform_bits;Absl::random_internal_fastmath;Absl::random_internal_iostream_state_saver;Absl::random_internal_traits;Absl::random_internal_uniform_helper;Absl::random_internal_wide_multiply;Absl::strings;Absl::type_traits"
  )

endif () # imported target Absl::random_seed_gen_exception
if (TARGET Absl::random_seed_gen_exception)

  set_target_properties(
    Absl::random_seed_gen_exception PROPERTIES INTERFACE_LINK_LIBRARIES
                                               "Absl::config;Absl::raw_logging_internal"
  )

endif () # imported target Absl::random_seed_sequences
if (TARGET Absl::random_seed_sequences)

  set_target_properties(
    Absl::random_seed_sequences
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::inlined_vector;Absl::nullability;Absl::random_internal_pool_urbg;Absl::random_internal_salted_seed_seq;Absl::random_internal_seed_material;Absl::random_seed_gen_exception;Absl::span;Absl::string_view"
  )

endif () # imported target Absl::random_internal_traits
if (TARGET Absl::random_internal_traits)

  set_target_properties(
    Absl::random_internal_traits PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config"
  )

endif () # imported target Absl::random_internal_distribution_caller
if (TARGET Absl::random_internal_distribution_caller)

  set_target_properties(
    Absl::random_internal_distribution_caller
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::config;Absl::utility;Absl::fast_type_id;Absl::type_traits"
  )

endif () # imported target Absl::random_internal_fast_uniform_bits
if (TARGET Absl::random_internal_fast_uniform_bits)

  set_target_properties(
    Absl::random_internal_fast_uniform_bits PROPERTIES INTERFACE_LINK_LIBRARIES
                                                       "Absl::config"
  )

endif () # imported target Absl::random_internal_seed_material
if (TARGET Absl::random_internal_seed_material)

  set_target_properties(
    Absl::random_internal_seed_material
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::core_headers;Absl::optional;Absl::random_internal_fast_uniform_bits;Absl::raw_logging_internal;Absl::span;Absl::strings;\$<LINK_ONLY:\$<\$<BOOL:>:-lbcrypt>>"
  )

endif () # imported target Absl::random_internal_pool_urbg
if (TARGET Absl::random_internal_pool_urbg)

  set_target_properties(
    Absl::random_internal_pool_urbg
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base;Absl::config;Absl::core_headers;Absl::endian;Absl::random_internal_randen;Absl::random_internal_seed_material;Absl::random_internal_traits;Absl::random_seed_gen_exception;Absl::raw_logging_internal;Absl::span"
  )

endif () # imported target Absl::random_internal_salted_seed_seq
if (TARGET Absl::random_internal_salted_seed_seq)

  set_target_properties(
    Absl::random_internal_salted_seed_seq
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::inlined_vector;Absl::optional;Absl::span;Absl::random_internal_seed_material;Absl::type_traits"
  )

endif () # imported target Absl::random_internal_iostream_state_saver
if (TARGET Absl::random_internal_iostream_state_saver)

  set_target_properties(
    Absl::random_internal_iostream_state_saver
    PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config;Absl::int128;Absl::type_traits"
  )

endif () # imported target Absl::random_internal_generate_real
if (TARGET Absl::random_internal_generate_real)

  set_target_properties(
    Absl::random_internal_generate_real
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::bits;Absl::random_internal_fastmath;Absl::random_internal_traits;Absl::type_traits"
  )

endif () # imported target Absl::random_internal_wide_multiply
if (TARGET Absl::random_internal_wide_multiply)

  set_target_properties(
    Absl::random_internal_wide_multiply PROPERTIES INTERFACE_LINK_LIBRARIES
                                                   "Absl::bits;Absl::config;Absl::int128"
  )

endif () # imported target Absl::random_internal_fastmath
if (TARGET Absl::random_internal_fastmath)

  set_target_properties(
    Absl::random_internal_fastmath PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::bits"
  )

endif () # imported target Absl::random_internal_nonsecure_base
if (TARGET Absl::random_internal_nonsecure_base)

  set_target_properties(
    Absl::random_internal_nonsecure_base
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::core_headers;Absl::inlined_vector;Absl::random_internal_pool_urbg;Absl::random_internal_salted_seed_seq;Absl::random_internal_seed_material;Absl::span;Absl::type_traits"
  )

endif () # imported target Absl::random_internal_pcg_engine
if (TARGET Absl::random_internal_pcg_engine)

  set_target_properties(
    Absl::random_internal_pcg_engine
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::int128;Absl::random_internal_fastmath;Absl::random_internal_iostream_state_saver;Absl::type_traits"
  )

endif () # imported target Absl::random_internal_randen_engine
if (TARGET Absl::random_internal_randen_engine)

  set_target_properties(
    Absl::random_internal_randen_engine
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::endian;Absl::random_internal_iostream_state_saver;Absl::random_internal_randen;Absl::raw_logging_internal;Absl::type_traits"
  )

endif () # imported target Absl::random_internal_platform
if (TARGET Absl::random_internal_platform)

  set_target_properties(
    Absl::random_internal_platform PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config"
  )

endif () # imported target Absl::random_internal_randen
if (TARGET Absl::random_internal_randen)

  set_target_properties(
    Absl::random_internal_randen
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::random_internal_platform;Absl::random_internal_randen_hwaes;Absl::random_internal_randen_slow"
  )

endif () # imported target Absl::random_internal_randen_slow
if (TARGET Absl::random_internal_randen_slow)

  set_target_properties(
    Absl::random_internal_randen_slow
    PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::random_internal_platform;Absl::config"
  )

endif () # imported target Absl::random_internal_randen_hwaes
if (TARGET Absl::random_internal_randen_hwaes)

  set_target_properties(
    Absl::random_internal_randen_hwaes
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::random_internal_platform;Absl::random_internal_randen_hwaes_impl;Absl::config;Absl::optional"
  )

endif () # imported target Absl::random_internal_randen_hwaes_impl
if (TARGET Absl::random_internal_randen_hwaes_impl)

  set_target_properties(
    Absl::random_internal_randen_hwaes_impl
    PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::random_internal_platform;Absl::config"
  )

endif () # imported target Absl::random_internal_distribution_test_util
if (TARGET Absl::random_internal_distribution_test_util)

  set_target_properties(
    Absl::random_internal_distribution_test_util
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::core_headers;Absl::raw_logging_internal;Absl::strings;Absl::str_format;Absl::span"
  )

endif () # imported target Absl::random_internal_uniform_helper
if (TARGET Absl::random_internal_uniform_helper)

  set_target_properties(
    Absl::random_internal_uniform_helper
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::config;Absl::random_internal_traits;Absl::type_traits"
  )

endif () # imported target Absl::status
if (TARGET Absl::status)

  set_target_properties(
    Absl::status
    PROPERTIES
      INTERFACE_COMPILE_DEFINITIONS "\$<\$<PLATFORM_ID:AIX>:_LINUX_SOURCE_COMPAT>"
      INTERFACE_LINK_LIBRARIES
      "Absl::atomic_hook;Absl::config;Absl::cord;Absl::core_headers;Absl::function_ref;Absl::inlined_vector;Absl::leak_check;Absl::memory;Absl::no_destructor;Absl::nullability;Absl::optional;Absl::raw_logging_internal;Absl::span;Absl::stacktrace;Absl::str_format;Absl::strerror;Absl::strings;Absl::symbolize"
  )

endif () # imported target Absl::statusor
if (TARGET Absl::statusor)

  set_target_properties(
    Absl::statusor
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base;Absl::config;Absl::core_headers;Absl::has_ostream_operator;Absl::nullability;Absl::raw_logging_internal;Absl::status;Absl::str_format;Absl::strings;Absl::type_traits;Absl::utility;Absl::variant"
  )

endif () # imported target Absl::string_view
if (TARGET Absl::string_view)

  set_target_properties(
    Absl::string_view
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base;Absl::config;Absl::core_headers;Absl::nullability;Absl::throw_delegate"
  )

endif () # imported target Absl::strings
if (TARGET Absl::strings)

  set_target_properties(
    Absl::strings
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::string_view;Absl::strings_internal;Absl::base;Absl::bits;Absl::charset;Absl::config;Absl::core_headers;Absl::endian;Absl::int128;Absl::memory;Absl::nullability;Absl::raw_logging_internal;Absl::throw_delegate;Absl::type_traits"
  )

endif () # imported target Absl::charset
if (TARGET Absl::charset)

  set_target_properties(
    Absl::charset PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config;Absl::string_view"
  )

endif () # imported target Absl::has_ostream_operator
if (TARGET Absl::has_ostream_operator)

  set_target_properties(
    Absl::has_ostream_operator PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config"
  )

endif () # imported target Absl::strings_internal
if (TARGET Absl::strings_internal)

  set_target_properties(
    Absl::strings_internal
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::core_headers;Absl::endian;Absl::raw_logging_internal;Absl::type_traits"
  )

endif () # imported target Absl::str_format
if (TARGET Absl::str_format)

  set_target_properties(
    Absl::str_format
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::core_headers;Absl::nullability;Absl::span;Absl::str_format_internal;Absl::string_view"
  )

endif () # imported target Absl::str_format_internal
if (TARGET Absl::str_format_internal)

  set_target_properties(
    Absl::str_format_internal
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::bits;Absl::strings;Absl::config;Absl::core_headers;Absl::fixed_array;Absl::inlined_vector;Absl::numeric_representation;Absl::type_traits;Absl::utility;Absl::int128;Absl::span"
  )

endif () # imported target Absl::cord_internal
if (TARGET Absl::cord_internal)

  set_target_properties(
    Absl::cord_internal
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base_internal;Absl::compressed_tuple;Absl::config;Absl::container_memory;Absl::compare;Absl::core_headers;Absl::crc_cord_state;Absl::endian;Absl::inlined_vector;Absl::layout;Absl::raw_logging_internal;Absl::strings;Absl::throw_delegate;Absl::type_traits"
  )

endif () # imported target Absl::cordz_update_tracker
if (TARGET Absl::cordz_update_tracker)

  set_target_properties(
    Absl::cordz_update_tracker PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config"
  )

endif () # imported target Absl::cordz_functions
if (TARGET Absl::cordz_functions)

  set_target_properties(
    Absl::cordz_functions
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::core_headers;Absl::exponential_biased;Absl::raw_logging_internal"
  )

endif () # imported target Absl::cordz_statistics
if (TARGET Absl::cordz_statistics)

  set_target_properties(
    Absl::cordz_statistics
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::core_headers;Absl::cordz_update_tracker;Absl::synchronization"
  )

endif () # imported target Absl::cordz_handle
if (TARGET Absl::cordz_handle)

  set_target_properties(
    Absl::cordz_handle
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base;Absl::config;Absl::no_destructor;Absl::raw_logging_internal;Absl::synchronization"
  )

endif () # imported target Absl::cordz_info
if (TARGET Absl::cordz_info)

  set_target_properties(
    Absl::cordz_info
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base;Absl::config;Absl::cord_internal;Absl::cordz_functions;Absl::cordz_handle;Absl::cordz_statistics;Absl::cordz_update_tracker;Absl::core_headers;Absl::inlined_vector;Absl::span;Absl::raw_logging_internal;Absl::stacktrace;Absl::synchronization;Absl::time"
  )

endif () # imported target Absl::cordz_sample_token
if (TARGET Absl::cordz_sample_token)

  set_target_properties(
    Absl::cordz_sample_token
    PROPERTIES INTERFACE_LINK_LIBRARIES
               "Absl::config;Absl::cordz_handle;Absl::cordz_info"
  )

endif () # imported target Absl::cordz_update_scope
if (TARGET Absl::cordz_update_scope)

  set_target_properties(
    Absl::cordz_update_scope
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::config;Absl::cord_internal;Absl::cordz_info;Absl::cordz_update_tracker;Absl::core_headers"
  )

endif () # imported target Absl::cord
if (TARGET Absl::cord)

  set_target_properties(
    Absl::cord
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base;Absl::config;Absl::cord_internal;Absl::cordz_functions;Absl::cordz_info;Absl::cordz_update_scope;Absl::cordz_update_tracker;Absl::core_headers;Absl::crc32c;Absl::crc_cord_state;Absl::endian;Absl::function_ref;Absl::inlined_vector;Absl::nullability;Absl::optional;Absl::raw_logging_internal;Absl::span;Absl::strings;Absl::type_traits"
  )

endif () # imported target Absl::graphcycles_internal
if (TARGET Absl::graphcycles_internal)

  set_target_properties(
    Absl::graphcycles_internal
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base;Absl::base_internal;Absl::config;Absl::core_headers;Absl::malloc_internal;Absl::raw_logging_internal"
  )

endif () # imported target Absl::kernel_timeout_internal
if (TARGET Absl::kernel_timeout_internal)

  set_target_properties(
    Absl::kernel_timeout_internal
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base;Absl::config;Absl::core_headers;Absl::raw_logging_internal;Absl::time"
  )

endif () # imported target Absl::synchronization
if (TARGET Absl::synchronization)

  set_target_properties(
    Absl::synchronization
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::graphcycles_internal;Absl::kernel_timeout_internal;Absl::atomic_hook;Absl::base;Absl::base_internal;Absl::config;Absl::core_headers;Absl::dynamic_annotations;Absl::malloc_internal;Absl::raw_logging_internal;Absl::stacktrace;Absl::symbolize;Absl::tracing_internal;Absl::time;Absl::tracing_internal;Threads::Threads"
  )

endif () # imported target Absl::time
if (TARGET Absl::time)

  set_target_properties(
    Absl::time
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::base;Absl::civil_time;Absl::core_headers;Absl::int128;Absl::raw_logging_internal;Absl::strings;Absl::time_zone"
  )

endif () # imported target Absl::civil_time

if (TARGET Absl::time_zone)

  set_target_properties(
    Absl::time_zone
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Threads::Threads;\$<\$<PLATFORM_ID:Darwin,iOS,tvOS,visionOS,watchOS>:-Wl,-framework,CoreFoundation>"
  )

endif () # imported target Absl::any
if (TARGET Absl::any)

  set_target_properties(
    Absl::any
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::bad_any_cast;Absl::config;Absl::core_headers;Absl::fast_type_id;Absl::type_traits;Absl::utility"
  )

endif () # imported target Absl::bad_any_cast
if (TARGET Absl::bad_any_cast)

  set_target_properties(
    Absl::bad_any_cast PROPERTIES INTERFACE_LINK_LIBRARIES
                                  "Absl::bad_any_cast_impl;Absl::config"
  )

endif () # imported target Absl::bad_any_cast_impl
if (TARGET Absl::bad_any_cast_impl)

  set_target_properties(
    Absl::bad_any_cast_impl PROPERTIES INTERFACE_LINK_LIBRARIES
                                       "Absl::config;Absl::raw_logging_internal"
  )

endif () # imported target Absl::span
if (TARGET Absl::span)

  set_target_properties(
    Absl::span
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::algorithm;Absl::config;Absl::core_headers;Absl::nullability;Absl::throw_delegate;Absl::type_traits"
  )

endif () # imported target Absl::optional
if (TARGET Absl::optional)

  set_target_properties(
    Absl::optional
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::bad_optional_access;Absl::base_internal;Absl::config;Absl::core_headers;Absl::memory;Absl::nullability;Absl::type_traits;Absl::utility"
  )

endif () # imported target Absl::bad_optional_access
if (TARGET Absl::bad_optional_access)

  set_target_properties(
    Absl::bad_optional_access PROPERTIES INTERFACE_LINK_LIBRARIES
                                         "Absl::config;Absl::raw_logging_internal"
  )

endif () # imported target Absl::bad_variant_access
if (TARGET Absl::bad_variant_access)

  set_target_properties(
    Absl::bad_variant_access PROPERTIES INTERFACE_LINK_LIBRARIES
                                        "Absl::config;Absl::raw_logging_internal"
  )

endif () # imported target Absl::variant
if (TARGET Absl::variant)

  set_target_properties(
    Absl::variant
    PROPERTIES
      INTERFACE_LINK_LIBRARIES
      "Absl::bad_variant_access;Absl::base_internal;Absl::config;Absl::core_headers;Absl::type_traits;Absl::utility"
  )

endif () # imported target Absl::compare
if (TARGET Absl::compare)

  set_target_properties(
    Absl::compare PROPERTIES INTERFACE_LINK_LIBRARIES
                             "Absl::config;Absl::core_headers;Absl::type_traits"
  )

endif () # imported target Absl::utility
if (TARGET Absl::utility)

  set_target_properties(
    Absl::utility PROPERTIES INTERFACE_LINK_LIBRARIES
                             "Absl::base_internal;Absl::config;Absl::type_traits"
  )

endif () # imported target Absl::if_constexpr
if (TARGET Absl::if_constexpr)

  set_target_properties(
    Absl::if_constexpr PROPERTIES INTERFACE_LINK_LIBRARIES "Absl::config"
  )
endif ()

if (NOT TARGET Absl::Absl)
  add_library(Absl::Absl INTERFACE IMPORTED)
  set_target_properties(Absl::Absl PROPERTIES INTERFACE_LINK_LIBRARIES "${ABSL_TARGETS}")

endif ()

find_package_handle_standard_args(absl REQUIRED_VARS ABSL_TARGETS)
