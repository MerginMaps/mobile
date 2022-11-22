# GPLv2 Licence

# not in linux input-SDK

if (LNX)
  find_path(Sqlite3_INCLUDE_DIR NAMES sqlite3.h)
  find_library(Sqlite3_LIBRARY NAMES sqlite3)
else ()
  find_path(
    Sqlite3_INCLUDE_DIR
    sqlite3.h
    "${INPUT_SDK_PATH_MULTI}/include"
    NO_DEFAULT_PATH
  )

  find_library(
    Sqlite3_LIBRARY
    NAMES sqlite3
    PATHS "${INPUT_SDK_PATH_MULTI}/lib"
    NO_DEFAULT_PATH
  )
endif ()

find_package_handle_standard_args(
  Sqlite3 REQUIRED_VARS Sqlite3_LIBRARY Sqlite3_INCLUDE_DIR
)

if (Sqlite3_FOUND AND NOT TARGET Sqlite3::Sqlite3)
  add_library(Sqlite3::Sqlite3 UNKNOWN IMPORTED)
  set_target_properties(
    Sqlite3::Sqlite3 PROPERTIES IMPORTED_LOCATION "${Sqlite3_LIBRARY}"
                                INTERFACE_INCLUDE_DIRECTORIES "${Sqlite3_INCLUDE_DIR}"
  )
endif ()

mark_as_advanced(Sqlite3_LIBRARY Sqlite3_INCLUDE_DIR)
