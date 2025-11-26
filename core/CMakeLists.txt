set(MM_CORE_SRCS
    coreutils.cpp
    merginapi.cpp
    merginapistatus.cpp
    merginsubscriptioninfo.cpp
    merginsubscriptionstatus.cpp
    merginservertype.cpp
    merginprojectstatusmodel.cpp
    merginuserauth.cpp
    merginuserinfo.cpp
    merginworkspaceinfo.cpp
    localprojectsmanager.cpp
    merginprojectmetadata.cpp
    project.cpp
    geodiffutils.cpp
    projectchecksumcache.cpp
)

set(MM_CORE_HDRS
    coreutils.h
    merginapi.h
    merginapistatus.h
    merginerrortypes.h
    merginsubscriptioninfo.h
    merginsubscriptionstatus.h
    merginservertype.h
    merginprojectstatusmodel.h
    merginuserauth.h
    merginuserinfo.h
    merginworkspaceinfo.h
    localprojectsmanager.h
    merginprojectmetadata.h
    project.h
    geodiffutils.h
    projectchecksumcache.h
    credentialstore.h
)

if (USE_MM_SERVER_API_KEY)
  # NOTE: merginsecrets.cpp should not be part of mm_core target sources
  if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/merginsecrets.cpp)
    message(
      FATAL_ERROR
        "core/merginsecrets.cpp doesn't exists. Decode it or set USE_MM_SERVER_API_KEY=FALSE"
    )
  endif ()
else ()
  message(
    STATUS
      "Using dummy MM_SERVER_API_KEY. You will not be able to call certain Mergin Maps API (e.g. register)"
  )
endif ()

if (USE_KEYCHAIN)
  set(MM_CORE_SRCS ${MM_CORE_SRCS} credentialstorekeychain.cpp)
  message(STATUS "Using QtKeychain to store credentials.")
else ()
  set(MM_CORE_SRCS ${MM_CORE_SRCS} credentialstoreplaintext.cpp)
  message(STATUS "Using QSettings to store credentials.")
endif ()

add_library(mm_core STATIC ${MM_CORE_SRCS} ${MM_CORE_HDRS})
target_include_directories(mm_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
  mm_core
  PRIVATE Qt6::Core
          Qt6::Network
          Qt6::NetworkAuth
          Qt6::Gui
          Geodiff::Geodiff
)

if (NOT USE_MM_SERVER_API_KEY)
  target_compile_definitions(mm_core PRIVATE USE_MERGIN_DUMMY_API_KEY)
endif ()
