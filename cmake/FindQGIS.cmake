# GPLv2 Licence

if (MACOS OR IOS)
  set(QGIS_DEFAULT_INCLUDE_PATH
      "${INPUT_SDK_PATH_MULTI}/QGIS.app/Contents/Frameworks/qgis_core.framework/Headers"
  )
  set(QGIS_DEFAULT_LIB_PATH "${INPUT_SDK_PATH_MULTI}/QGIS.app/Contents/Frameworks")
  set(QGIS_DEFAULT_PROVIDER_PATH "${INPUT_SDK_PATH_MULTI}/QGIS.app/Contents/PlugIns/qgis")
elseif (ANDROID)
  set(QGIS_DEFAULT_INCLUDE_PATH "${INPUT_SDK_PATH_MULTI}/include/qgis")
  set(QGIS_DEFAULT_LIB_PATH "${INPUT_SDK_PATH_MULTI}/lib")
  set(QGIS_DEFAULT_PROVIDER_PATH "${QGIS_DEFAULT_LIB_PATH}")
elseif (LNX)
  set(QGIS_DEFAULT_INCLUDE_PATH "${INPUT_SDK_PATH_MULTI}/include/qgis")
  set(QGIS_DEFAULT_LIB_PATH "${INPUT_SDK_PATH_MULTI}/lib")
  set(QGIS_DEFAULT_PROVIDER_PATH "${QGIS_DEFAULT_LIB_PATH}/qgis/plugins")
else () # windows
  set(QGIS_DEFAULT_INCLUDE_PATH "${INPUT_SDK_PATH_MULTI}/include/qgis")
  set(QGIS_DEFAULT_LIB_PATH "${INPUT_SDK_PATH_MULTI}/lib")
  set(QGIS_DEFAULT_PROVIDER_PATH "${QGIS_DEFAULT_LIB_PATH}")
endif ()

find_path(
  QGIS_INCLUDE_DIR
  qgsproject.h
  "${QGIS_DEFAULT_INCLUDE_PATH}"
  NO_DEFAULT_PATH
)

find_library(
  QGIS_CORE_LIBRARY
  NAMES qgis_core
  PATHS "${QGIS_DEFAULT_LIB_PATH}"
  NO_DEFAULT_PATH
)

set(QGIS_REQUIRED_VARS QGIS_CORE_LIBRARY QGIS_INCLUDE_DIR)
set(QGIS_PROVIDERS_LIST
    authmethod_basic
    authmethod_esritoken
    authmethod_identcert
    authmethod_oauth2
    authmethod_pkcs12
    authmethod_pkipaths
    provider_arcgisfeatureserver
    provider_arcgismapserver
    provider_delimitedtext
    provider_spatialite
    provider_virtuallayer
    provider_wcs
    provider_wfs
    provider_wms
    provider_postgres
)

foreach (provider ${QGIS_PROVIDERS_LIST})
  find_library(
    QGIS_${provider}_LIBRARY
    NAMES ${provider}_a
    PATHS "${QGIS_DEFAULT_PROVIDER_PATH}"
    NO_DEFAULT_PATH
  )
  set(QGIS_REQUIRED_VARS ${QGIS_REQUIRED_VARS} QGIS_${provider}_LIBRARY)
endforeach ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QGIS REQUIRED_VARS ${QGIS_REQUIRED_VARS})

if (QGIS_FOUND AND NOT TARGET QGIS::Core)
  if (APPLE)
    add_library(QGIS::Core INTERFACE IMPORTED)
    set_target_properties(
      QGIS::Core
      PROPERTIES INTERFACE_LINK_LIBRARIES
                 "-F${QGIS_DEFAULT_LIB_PATH} -framework qgis_core"
                 INTERFACE_INCLUDE_DIRECTORIES "${QGIS_INCLUDE_DIR}"
    )
  else ()
    add_library(QGIS::Core UNKNOWN IMPORTED)
    set_target_properties(
      QGIS::Core PROPERTIES IMPORTED_LOCATION "${QGIS_CORE_LIBRARY}"
                            INTERFACE_INCLUDE_DIRECTORIES "${QGIS_INCLUDE_DIR}"
    )
  endif ()

  foreach (provider ${QGIS_PROVIDERS_LIST})
    add_library(QGIS::${provider} STATIC IMPORTED)
    set_target_properties(
      QGIS::${provider} PROPERTIES IMPORTED_LOCATION "${QGIS_${provider}_LIBRARY}"
    )
  endforeach ()

endif ()

mark_as_advanced(QGIS_INCLUDE_DIR QGIS_CORE_LIBRARY)
foreach (provider ${QGIS_PROVIDERS_LIST})
  mark_as_advanced(QGIS_${provider}_LIBRARY)
endforeach ()
