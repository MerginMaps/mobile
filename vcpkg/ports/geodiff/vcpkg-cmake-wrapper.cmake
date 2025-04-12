include(SelectLibraryConfigurations)
select_library_configurations(Geodiff)

find_path(Geodiff_INCLUDE_DIR 
    NAMES geodiff.h 
    PATHS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include"
    NO_DEFAULT_PATH
)

if(NOT Geodiff_LIBRARY)
  find_library(Geodiff_LIBRARY_RELEASE NAMES geodiff
    PATHS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib" 
    NO_DEFAULT_PATH
  )
  find_library(Geodiff_LIBRARY_DEBUG NAMES geodiff
    PATHS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib"
    NO_DEFAULT_PATH
  )
  select_library_configurations(Geodiff)
  mark_as_advanced(Geodiff_LIBRARY_RELEASE Geodiff_LIBRARY_DEBUG)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Geodiff
    REQUIRED_VARS Geodiff_LIBRARY Geodiff_INCLUDE_DIR
)

find_package(unofficial-sqlite3 CONFIG QUIET)

if (Geodiff_FOUND AND NOT TARGET Geodiff::Geodiff)
  add_library(Geodiff::Geodiff UNKNOWN IMPORTED)
  set_target_properties( Geodiff::Geodiff PROPERTIES 
      INTERFACE_LINK_LIBRARIES unofficial::sqlite3::sqlite3
  )

  if(Geodiff_INCLUDE_DIR)
    set_target_properties(Geodiff::Geodiff PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${Geodiff_INCLUDE_DIR}"
    )
  endif()
  
  if(EXISTS "${Geodiff_LIBRARY}")
    set_target_properties(Geodiff::Geodiff PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      IMPORTED_LOCATION "${Geodiff_LIBRARY}")
  endif()
  
  if(EXISTS "${Geodiff_LIBRARY_RELEASE}")
    set_property(TARGET Geodiff::Geodiff APPEND PROPERTY
      IMPORTED_CONFIGURATIONS RELEASE)
    set_target_properties(Geodiff::Geodiff PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
      IMPORTED_LOCATION_RELEASE "${Geodiff_LIBRARY_RELEASE}")
  endif()
  
  if(EXISTS "${Geodiff_LIBRARY_DEBUG}")
    set_property(TARGET Geodiff::Geodiff APPEND PROPERTY
      IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(Geodiff::Geodiff PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
      IMPORTED_LOCATION_DEBUG "${Geodiff_LIBRARY_DEBUG}")
  endif()
endif ()
