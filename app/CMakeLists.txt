# ########################################################################################
# SOURCES of Mergin Maps Target
# ########################################################################################

set(MM_SRCS
    attributes/attributecontroller.cpp
    attributes/attributedata.cpp
    attributes/attributeformmodel.cpp
    attributes/attributeformproxymodel.cpp
    attributes/attributepreviewcontroller.cpp
    attributes/attributetabmodel.cpp
    attributes/attributetabproxymodel.cpp
    attributes/fieldvalidator.cpp
    attributes/mixedattributevalue.cpp
    attributes/rememberattributescontroller.cpp
    layer/layerdetaildata.cpp
    layer/layerdetaillegendimageprovider.cpp
    layer/layertreeflatmodel.cpp
    layer/layertreeflatmodelpixmapprovider.cpp
    layer/layertreeflatsortfiltermodel.cpp
    layer/layertreemodel.cpp
    layer/layertreemodelpixmapprovider.cpp
    layer/layertreesortfiltermodel.cpp
    map/inputcoordinatetransformer.cpp
    map/inputmapcanvasmap.cpp
    map/inputmapsettings.cpp
    map/inputmaptransform.cpp
    maptools/abstractmaptool.cpp
    maptools/recordingmaptool.cpp
    maptools/splittingmaptool.cpp
    maptools/measurementmaptool.cpp
    ios/iosimagepicker.cpp
    ios/iosutils.cpp
    position/providers/abstractpositionprovider.cpp
    position/providers/internalpositionprovider.cpp
    position/providers/positionprovidersmodel.cpp
    position/providers/simulatedpositionprovider.cpp
    position/tracking/abstracttrackingbackend.cpp
    position/tracking/internaltrackingbackend.cpp
    position/tracking/positiontrackinghighlight.cpp
    position/tracking/positiontrackingmanager.cpp
    position/geoposition.cpp
    position/mapposition.cpp
    position/positiondirection.cpp
    position/positionkit.cpp
    activelayer.cpp
    activeproject.cpp
    androidutils.cpp
    mapsketchingcontroller.cpp
    appsettings.cpp
    autosynccontroller.cpp
    bluetoothdiscoverymodel.cpp
    qrcodedecoder.cpp
    changelogmodel.cpp
    compass.cpp
    featurelayerpair.cpp
    featuresmodel.cpp
    fieldsmodel.cpp
    filtercontroller.cpp
    guidelinecontroller.cpp
    hapticsmodel.cpp
    identifykit.cpp
    imageutils.cpp
    inputexpressionfunctions.cpp
    inputhelp.cpp
    inputprojutils.cpp
    inpututils.cpp
    inpututils_p.cpp
    invitationsmodel.cpp
    invitationsproxymodel.cpp
    layerfeaturesmodel.cpp
    layersmodel.cpp
    recordinglayersproxymodel.cpp
    main.cpp
    mapthemesmodel.cpp
    multieditmanager.cpp
    notificationmodel.cpp
    photosketchingcontroller.cpp
    projectsmodel.cpp
    projectsproxymodel.cpp
    projectwizard.cpp
    relationfeaturesmodel.cpp
    relationreferencefeaturesmodel.cpp
    scalebarkit.cpp
    snaputils.cpp
    staticfeaturesmodel.cpp
    streamingintervaltype.cpp
    synchronizationerror.cpp
    synchronizationmanager.cpp
    valuerelationfeaturesmodel.cpp
    variablesmanager.cpp
    workspacesmodel.cpp
    workspacesproxymodel.cpp
)

set(MM_HDRS
    attributes/attributecontroller.h
    attributes/attributedata.h
    attributes/attributeformmodel.h
    attributes/attributeformproxymodel.h
    attributes/attributepreviewcontroller.h
    attributes/attributetabmodel.h
    attributes/attributetabproxymodel.h
    attributes/fieldvalidator.h
    attributes/mixedattributevalue.h
    attributes/rememberattributescontroller.h
    layer/layerdetaildata.h
    layer/layerdetaillegendimageprovider.h
    layer/layertreeflatmodel.h
    layer/layertreeflatmodelpixmapprovider.h
    layer/layertreeflatsortfiltermodel.h
    layer/layertreemodel.h
    layer/layertreemodelpixmapprovider.h
    layer/layertreesortfiltermodel.h
    map/inputcoordinatetransformer.h
    map/inputmapcanvasmap.h
    map/inputmapsettings.h
    map/inputmaptransform.h
    maptools/abstractmaptool.h
    maptools/recordingmaptool.h
    maptools/splittingmaptool.h
    maptools/measurementmaptool.h
    ios/iosimagepicker.h
    ios/iosutils.h
    position/providers/abstractpositionprovider.h
    position/providers/internalpositionprovider.h
    position/providers/positionprovidersmodel.h
    position/providers/simulatedpositionprovider.h
    position/tracking/abstracttrackingbackend.h
    position/tracking/internaltrackingbackend.h
    position/tracking/positiontrackinghighlight.h
    position/tracking/positiontrackingmanager.h
    position/geoposition.h
    position/mapposition.h
    position/positiondirection.h
    position/positionkit.h
    activelayer.h
    activeproject.h
    androidutils.h
    mapsketchingcontroller.h
    appsettings.h
    autosynccontroller.h
    bluetoothdiscoverymodel.h
    qrcodedecoder.h
    changelogmodel.h
    compass.h
    enumhelper.h
    featurelayerpair.h
    featuresmodel.h
    fieldsmodel.h
    filtercontroller.h
    guidelinecontroller.h
    hapticsmodel.h
    identifykit.h
    imageutils.h
    inputexpressionfunctions.h
    inputhelp.h
    inputprojutils.h
    inpututils.h
    inpututils_p.h
    invitationsmodel.h
    invitationsproxymodel.h
    layerfeaturesmodel.h
    layersmodel.h
    recordinglayersproxymodel.h
    mapthemesmodel.h
    multieditmanager.h
    notificationmodel.h
    photosketchingcontroller.h
    projectsmodel.h
    projectsproxymodel.h
    projectwizard.h
    relationfeaturesmodel.h
    relationreferencefeaturesmodel.h
    scalebarkit.h
    snaputils.h
    staticfeaturesmodel.h
    streamingintervaltype.h
    synchronizationerror.h
    synchronizationmanager.h
    synchronizationoptions.h
    valuerelationfeaturesmodel.h
    variablesmanager.h
    workspacesmodel.h
    workspacesproxymodel.h
    mmstyle.h
)

if (HAVE_BLUETOOTH)
  set(MM_SRCS ${MM_SRCS} position/providers/bluetoothpositionprovider.cpp)

  set(MM_HDRS ${MM_HDRS} position/providers/bluetoothpositionprovider.h)
endif ()

if (ENABLE_TESTS)
  set(MM_SRCS
      ${MM_SRCS}
      test/inputtests.cpp
      test/testattributecontroller.cpp
      test/testattributepreviewcontroller.cpp
      test/testcoreutils.cpp
      test/testformeditors.cpp
      test/testidentifykit.cpp
      test/testimageutils.cpp
      test/testlayertree.cpp
      test/testlinks.cpp
      test/testmaptools.cpp
      test/testmerginapi.cpp
      test/testmodels.cpp
      test/testposition.cpp
      test/testrememberattributescontroller.cpp
      test/testscalebarkit.cpp
      test/testsketching.cpp
      test/testutils.cpp
      test/testutilsfunctions.cpp
      test/testvariablesmanager.cpp
      test/testactiveproject.cpp
      test/testprojectchecksumcache.cpp
      test/testmultieditmanager.cpp
  )

  set(MM_HDRS
      ${MM_HDRS}
      test/inputtests.h
      test/testattributecontroller.h
      test/testattributepreviewcontroller.h
      test/testcoreutils.h
      test/testformeditors.h
      test/testidentifykit.h
      test/testimageutils.h
      test/testlayertree.h
      test/testlinks.h
      test/testmaptools.h
      test/testmerginapi.h
      test/testmodels.h
      test/testposition.h
      test/testrememberattributescontroller.h
      test/testscalebarkit.h
      test/testsketching.h
      test/testutils.h
      test/testutilsfunctions.h
      test/testvariablesmanager.h
      test/testactiveproject.h
      test/testprojectchecksumcache.h
      test/testmultieditmanager.h
  )

  if (NOT USE_MM_SERVER_API_KEY)
    set_property(
      SOURCE test/testmerginapi.cpp PROPERTY COMPILE_DEFINITIONS USE_MERGIN_DUMMY_API_KEY
    )
  endif ()
endif ()

if (IOS)
  set(MM_HDRS
      ${MM_HDRS}
      ios/iosinterface.h
      ios/iosviewdelegate.h
      position/tracking/iostrackingbackend.h
      position/tracking/iostrackingbackendimpl.h
  )

  set(MM_SRCS
      ${MM_SRCS}
      ios/iosinterface.mm
      ios/iosviewdelegate.mm
      ios/iosimagepicker.mm
      ios/iosutils.mm
      position/tracking/iostrackingbackend.cpp
      position/tracking/iostrackingbackend.mm
      position/tracking/iostrackingbackendimpl.mm
  )
endif ()

if (ANDROID)
  set(MM_HDRS
      ${MM_HDRS}
      position/tracking/androidtrackingbackend.h
      position/tracking/androidtrackingbroadcast.h
      position/providers/androidpositionprovider.h
  )

  set(MM_SRCS
      ${MM_SRCS}
      position/tracking/androidtrackingbackend.cpp
      position/tracking/androidtrackingbroadcast.cpp
      position/providers/androidpositionprovider.cpp
  )
endif ()

qt_add_resources(
  MM_SRCS
  icons/icons.qrc
  images/qgis/qgisicons.qrc
  images/images.qrc
  i18n/input_i18n.qrc
  fonts/fonts.qrc
  sounds/sounds.qrc
)

# ########################################################################################
# Mergin Maps Target
# ########################################################################################

qt_add_executable(
  MerginMaps
  ${MM_SRCS}
  ${MM_HDRS}
  main.cpp
)

set_target_properties(MerginMaps PROPERTIES WIN32_EXECUTABLE TRUE)

target_include_directories(
  MerginMaps
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/
         ${CMAKE_CURRENT_SOURCE_DIR}/attributes
         ${CMAKE_CURRENT_SOURCE_DIR}/map
         ${CMAKE_CURRENT_SOURCE_DIR}/layer
         ${CMAKE_CURRENT_SOURCE_DIR}/maptools
         ${CMAKE_CURRENT_SOURCE_DIR}/position
)

qt_policy(SET QTP0002 NEW)

if (ANDROID)
  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml)
    file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml)
  endif ()
  configure_file(
    ${CMAKE_SOURCE_DIR}/cmake_templates/AndroidManifest.xml.in
    ${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml @ONLY
  )

  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/android/build.gradle)
    file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/android/build.gradle)
  endif ()
  configure_file(
    ${CMAKE_SOURCE_DIR}/cmake_templates/build.gradle.in
    ${CMAKE_CURRENT_SOURCE_DIR}/android/build.gradle @ONLY
  )

  set_target_properties(
    MerginMaps
    PROPERTIES QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
               QT_ANDROID_TARGET_SDK_VERSION ${MM_ANDROID_TARGET_SDK_VERSION}
               QT_ANDROID_MIN_SDK_VERSION ${MM_ANDROID_MIN_SDK_VERSION}
               QT_ANDROID_VERSION_CODE ${MM_VERSION_CODE}
               QT_ANDROID_VERSION_NAME ${MM_VERSION}
  )
endif ()

if (MACOS)
  set_target_properties(
    MerginMaps
    PROPERTIES MACOSX_BUNDLE TRUE
               MACOSX_RPATH TRUE
               MACOSX_BUNDLE_GUI_IDENTIFIER LutraConsultingLtd.Input
               MACOSX_BUNDLE_INFO_PLIST
               ${CMAKE_SOURCE_DIR}/cmake_templates/macOsInfo.plist.in
               MACOSX_BUNDLE_SHORT_VERSION_STRING ${MM_VERSION_CODE}
               XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@loader_path/Libraries"
               XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME TRUE
  )
endif ()

if (IOS)
  # qgis resources
  set(IOS_CRS_DIR ${CMAKE_CURRENT_BINARY_DIR}/android/assets/qgis-data/resources)
  target_sources(MerginMaps PRIVATE ${IOS_CRS_DIR})
  set_source_files_properties(
    ${IOS_CRS_DIR} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/qgis-data
  )

  # proj
  set(IOS_PROJ_DIR ${CMAKE_CURRENT_BINARY_DIR}/android/assets/qgis-data/proj)
  target_sources(MerginMaps PRIVATE ${IOS_PROJ_DIR})
  set_source_files_properties(
    ${IOS_PROJ_DIR} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/qgis-data
  )

  # app icon
  set(IOS_ASSET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ios/Images.xcassets)
  target_sources(MerginMaps PRIVATE ${IOS_ASSET_DIR})
  set_source_files_properties(
    ${IOS_ASSET_DIR} PROPERTIES MACOSX_PACKAGE_LOCATION Resources
  )

  # launch screen
  set_target_properties(
    MerginMaps
    PROPERTIES QT_IOS_LAUNCH_SCREEN
               ${CMAKE_CURRENT_SOURCE_DIR}/ios/launchscreen/MMLaunchScreen.storyboard
  )

  # entitlements file
  set(ENTITLEMENTS_FILE ${CMAKE_SOURCE_DIR}/cmake_templates/mm.entitlements)
  configure_file(
    ${ENTITLEMENTS_FILE} ${CMAKE_CURRENT_BINARY_DIR}/mm.entitlements COPYONLY
  )
  set(ENTITLEMENTS_FILE_IN_BUILD ${CMAKE_CURRENT_BINARY_DIR}/mm.entitlements)
  target_sources(MerginMaps PRIVATE ${ENTITLEMENTS_FILE})
  set_source_files_properties(
    ${ENTITLEMENTS_FILE} PROPERTIES XCODE_LAST_KNOWN_FILE_TYPE "text.plist.entitlements"
  )

  message(STATUS "iOS signing entitlements file: ${ENTITLEMENTS_FILE_IN_BUILD}")

  set_target_properties(
    MerginMaps
    PROPERTIES MACOSX_BUNDLE TRUE
               MACOSX_BUNDLE_INFO_PLIST
               ${CMAKE_SOURCE_DIR}/cmake_templates/iOSInfo.plist.in
               MACOSX_BUNDLE_GUI_IDENTIFIER "LutraConsultingLtd.Input"
               MACOSX_BUNDLE_SHORT_VERSION_STRING ${MM_VERSION}
               MACOSX_BUNDLE_BUNDLE_VERSION ${MM_VERSION_CODE}
               XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "79QMH2QRAH"
               XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon"
               XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${ENTITLEMENTS_FILE_IN_BUILD}"
  )

  if (IOS_USE_PRODUCTION_SIGNING)

    # use production signing profile
    set_target_properties(
      MerginMaps
      PROPERTIES XCODE_ATTRIBUTE_PROVISIONING_PROFILE_SPECIFIER
                 "LutraConsultingLtd.Input.AppStore"
                 XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY
                 "iPhone Distribution: LUTRA CONSULTING LIMITED (79QMH2QRAH)"
                 XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual"
    )
    message(STATUS "Using manual code sign with production certificate")

  else ()

    # use development profiles - automatically manage signing
    set_target_properties(
      MerginMaps PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Automatic"
    )
    message(STATUS "Using automatic code sign")
  endif ()
endif ()

# ########################################################################################
# LINKING Mergin Maps Target
# ########################################################################################

target_link_libraries(MerginMaps PUBLIC mm_core)

# at least on Linux qgis_core needs to be linked before Qt libs, otherwise one gets "DSO
# missing from command line" linker error about missing symbol from qt6::core5compat
# library - probably if our code does not use anything from that library, it is not added
# (linking order matters)
target_link_libraries(MerginMaps PUBLIC QGIS::Core)

target_link_libraries(
  MerginMaps
  PUBLIC Qt6::Quick
         Qt6::Qml
         Qt6::Xml
         Qt6::Concurrent
         Qt6::Positioning
         Qt6::Sensors
         Qt6::QuickControls2
         Qt6::Network
         Qt6::NetworkAuth
         Qt6::Svg
         Qt6::Sql
         Qt6::Core
         Qt6::Multimedia
         Qt6::Widgets
)

if (HAVE_BLUETOOTH)
  target_link_libraries(MerginMaps PUBLIC Qt6::Bluetooth)
endif ()

if (NOT IOS)
  target_link_libraries(MerginMaps PUBLIC Qt6::PrintSupport)
endif ()

target_link_libraries(MerginMaps PUBLIC PROJ::proj ZXing::ZXing)

target_link_libraries(
  MerginMaps
  PUBLIC QGIS::authmethod_basic
         QGIS::authmethod_esritoken
         QGIS::authmethod_identcert
         QGIS::authmethod_oauth2
         QGIS::authmethod_pkcs12
         QGIS::authmethod_pkipaths
         QGIS::provider_arcgisfeatureserver
         QGIS::provider_arcgismapserver
         QGIS::provider_delimitedtext
         QGIS::provider_spatialite
         QGIS::provider_virtuallayer
         QGIS::provider_wcs
         QGIS::provider_wfs
         QGIS::provider_wms
         QGIS::provider_postgres
)

if (LNX)
  # needs to be added again because of the cycling dependencies between qgis_core and
  # providers and auth methods
  target_link_libraries(MerginMaps PUBLIC QGIS::Core)
endif ()

target_link_libraries(
  MerginMaps
  PUBLIC Qt6Keychain::Qt6Keychain
         qca
         GDAL::GDAL
         PostgreSQL::PostgreSQL
         Spatialite::Spatialite
)

target_link_libraries(
  MerginMaps
  PUBLIC Spatialindex::Spatialindex
         protobuf::libprotobuf-lite
         Exiv2::exiv2lib
         ZLIB::ZLIB
         libzip::zip
         poly2tri::poly2tri
         meshoptimizer::meshoptimizer
)

if (WIN)
  target_link_libraries(MerginMaps PUBLIC GEOS::geos_c Qt6::Core5Compat)
endif ()

if (IOS OR MACOS)
  target_link_libraries(MerginMaps PUBLIC Tasn1::Tasn1)
endif ()

if (ANDROID)
  set_property(
    TARGET MerginMaps PROPERTY QT_ANDROID_EXTRA_LIBS ${OpenSSL_SSL_LIBRARY}
                               ${OpenSSL_CRYPTO_LIBRARY}
  )
endif ()

if (MACOS OR IOS)
  target_link_libraries(
    MerginMaps PUBLIC AppleFrameworks::Security AppleFrameworks::CoreFoundation
                      AppleFrameworks::SystemConfiguration
  )
endif ()

if (IOS)
  target_link_libraries(
    MerginMaps PUBLIC AppleFrameworks::CoreLocation AppleFrameworks::CoreHaptics
  )
  # TODO is this needed? this change requires cmake 3.28+
  # qt_add_ios_ffmpeg_libraries(MerginMaps) # Qt Multimedia
endif ()

if (WIN)
  target_link_libraries(
    MerginMaps
    PUBLIC WinLibraries::avcodec
           WinLibraries::avdevice
           WinLibraries::avformat
           WinLibraries::avutil
           WinLibraries::swresample
           WinLibraries::swscale
           WinLibraries::openh264
  )

  target_link_libraries(MerginMaps PUBLIC expat::expat)
endif ()

if (ENABLE_TESTS)
  target_link_libraries(MerginMaps PUBLIC Qt6::Test)
endif ()

if (ANDROID)
  target_link_libraries(MerginMaps PUBLIC "-ljnigraphics")
endif ()

# ########################################################################################
# QML
# ########################################################################################

target_compile_definitions(
  MerginMaps PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
)

add_subdirectory(qml)

qt_policy(SET QTP0004 NEW)

# make sure the QML files are not prefixed with "qml/" in the qmldir
set(MM_QML_SRCS)
foreach (src ${MM_QML})
  set(MM_QML_SRCS ${MM_QML_SRCS} qml/${src})
  set_source_files_properties(qml/${src} PROPERTIES QT_RESOURCE_ALIAS ${src})
endforeach ()

# For some reason, this call cannot be in qml/CMakeLists.txt
qt_add_qml_module(
  MerginMaps
  URI MMInput
  VERSION 1.0
  QML_FILES ${MM_QML_SRCS}
  RESOURCE_PREFIX /com.merginmaps/imports
)
