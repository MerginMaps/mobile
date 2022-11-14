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

set(MM_QGSQUICK_SRCS
    qgsquickcoordinatetransformer.cpp
    qgsquickmapcanvasmap.cpp
    qgsquickmapsettings.cpp
    qgsquickmaptransform.cpp
    qgsquickutils.cpp
)

set(MM_QGSQUICK_SRCS
    qgsquickcoordinatetransformer.h
    qgsquickmapcanvasmap.h
    qgsquickmapsettings.h
    qgsquickmaptransform.h
    qgis_quick.h
    qgsquickutils.h
)


if(USE_MM_SERVER_API_KEY)
    set(MM_CORE_SRCS ${MM_CORE_SRCS} merginsecrets.cpp)
endif()

add_library(mm_core OBJECT ${MM_CORE_SRCS} ${MM_CORE_HDRS})

if(NOT USE_MM_SERVER_API_KEY)
    target_compile_definitions(mm_core PRIVATE USE_MERGIN_DUMMY_API_KEY)
endif()