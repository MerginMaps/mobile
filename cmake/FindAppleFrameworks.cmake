# GPLv2 Licence

if (NOT APPLE)
  message(FATAL_ERROR "Apple frameworks are not available on this platform")
endif ()

# Defines
#
# macOS + iOS
#
# AppleFrameworks::Security AppleFrameworks::CoreFoundation
# AppleFrameworks::SystemConfiguration
#
# iOS AppleFrameworks::StoreKit AppleFrameworks::Foundation

set(APPLE_FRAMEWORKS Security CoreFoundation SystemConfiguration)

if (HAVE_APPLE_PURCHASING)
  set(APPLE_FRAMEWORKS ${APPLE_FRAMEWORKS} Foundation StoreKit)
endif ()

foreach (framework ${APPLE_FRAMEWORKS})
  find_library(APPLE_${framework}_LIBRARY NAMES ${framework})
  set(APPLE_REQUIRED_VARS ${APPLE_REQUIRED_VARS} APPLE_${framework}_LIBRARY)
endforeach ()

find_package_handle_standard_args(AppleFrameworks REQUIRED_VARS ${APPLE_REQUIRED_VARS})

if (AppleFrameworks_FOUND AND NOT TARGET AppleFrameworks::CoreFoundation)
  foreach (framework ${APPLE_FRAMEWORKS})
    add_library(AppleFrameworks::${framework} INTERFACE IMPORTED)
    set_target_properties(
      AppleFrameworks::${framework} PROPERTIES INTERFACE_LINK_LIBRARIES
                                               "-framework ${framework}"
    )
  endforeach ()
endif ()

foreach (framework ${APPLE_FRAMEWORKS})
  mark_as_advanced(${APPLE_${framework}_LIBRARY})
endforeach ()
