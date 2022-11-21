set(MM_CORE_SRCS
    coreutils.cpp
    merginapi.cpp
    merginapistatus.cpp
    merginsubscriptioninfo.cpp
    merginsubscriptionstatus.cpp
    merginsubscriptiontype.cpp
    merginprojectstatusmodel.cpp
    merginuserauth.cpp
    merginuserinfo.cpp
    localprojectsmanager.cpp
    merginprojectmetadata.cpp
    project.cpp
    geodiffutils.cpp
)

set(MM_CORE_HDRS
    coreutils.h
    merginapi.h
    merginapistatus.h
    merginsubscriptioninfo.h
    merginsubscriptionstatus.h
    merginsubscriptiontype.h
    merginprojectstatusmodel.h
    merginuserauth.h
    merginuserinfo.h
    localprojectsmanager.h
    merginprojectmetadata.h
    project.h
    geodiffutils.h
)

if (USE_MM_SERVER_API_KEY)
  if (EXISTS merginsecrets.cpp)
    set(MM_CORE_SRCS ${MM_CORE_SRCS} merginsecrets.cpp)
  else ()
    message(
      FATAL_ERROR
        "core/merginsecrets.cpp doesn't exists. Decode it or set USE_MM_SERVER_API_KEY=FALSE"
    )
  endif ()
endif ()

add_library(mm_core OBJECT ${MM_CORE_SRCS} ${MM_CORE_HDRS})
target_include_directories(mm_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mm_core PRIVATE Qt6::Core Qt6::Network Geodiff::Geodiff)

if (NOT USE_MM_SERVER_API_KEY)
  target_compile_definitions(mm_core PRIVATE USE_MERGIN_DUMMY_API_KEY)
endif ()
