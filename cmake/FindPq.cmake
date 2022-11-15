# GPLv2 Licence

find_path(Pq_INCLUDE_DIR postgres_ext.h
  "${INPUT_SDK_PATH}/include"
  NO_DEFAULT_PATH
)
  
find_library(Pq_LIBRARY 
  NAMES pq
  PATHS "${INPUT_SDK_PATH}/lib"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(
  Pq
  REQUIRED_VARS Pq_LIBRARY Pq_INCLUDE_DIR
)

if(Pq_FOUND AND NOT TARGET Pq::Pq)
  add_library(Pq::Pq STATIC IMPORTED)
  set_target_properties(Pq::Pq PROPERTIES
    IMPORTED_LOCATION "${Pq_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Pq_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  Pq_LIBRARY
  Pq_INCLUDE_DIR
)