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
    ios/iosimagepicker.cpp
    ios/iosutils.cpp
    position/abstractpositionprovider.cpp
    position/internalpositionprovider.cpp
    position/mapposition.cpp
    position/positiondirection.cpp
    position/positionkit.cpp
    position/positionprovidersmodel.cpp
    position/simulatedpositionprovider.cpp
    activelayer.cpp
    activeproject.cpp
    androidutils.cpp
    appsettings.cpp
    autosynccontroller.cpp
    bluetoothdiscoverymodel.cpp
    codescanner.cpp
    compass.cpp
    featurelayerpair.cpp
    featuresmodel.cpp
    fieldsmodel.cpp
    guidelinecontroller.cpp
    identifykit.cpp
    imageutils.cpp
    inputexpressionfunctions.cpp
    inputhelp.cpp
    inputprojutils.cpp
    inpututils.cpp
    invitationsmodel.cpp
    invitationsproxymodel.cpp
    layersmodel.cpp
    layersproxymodel.cpp
    main.cpp
    mapthemesmodel.cpp
    projectsmodel.cpp
    projectsproxymodel.cpp
    projectwizard.cpp
    purchasing.cpp
    qrdecoder.cpp
    relationfeaturesmodel.cpp
    relationreferencefeaturesmodel.cpp
    scalebarkit.cpp
    snaputils.cpp
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
    ios/iosimagepicker.h
    ios/iosutils.h
    position/abstractpositionprovider.h
    position/internalpositionprovider.h
    position/mapposition.h
    position/positiondirection.h
    position/positionkit.h
    position/positionprovidersmodel.h
    position/simulatedpositionprovider.h
    activelayer.h
    activeproject.h
    androidutils.h
    appsettings.h
    autosynccontroller.h
    bluetoothdiscoverymodel.h
    codescanner.h
    compass.h
    featurelayerpair.h
    featuresmodel.h
    fieldsmodel.h
    guidelinecontroller.h
    identifykit.h
    imageutils.h
    inputexpressionfunctions.h
    inputhelp.h
    inputprojutils.h
    inpututils.h
    invitationsmodel.h
    invitationsproxymodel.h
    layersmodel.h
    layersproxymodel.h
    mapthemesmodel.h
    projectsmodel.h
    projectsproxymodel.h
    projectwizard.h
    purchasing.h
    qrdecoder.h
    relationfeaturesmodel.h
    relationreferencefeaturesmodel.h
    scalebarkit.h
    snaputils.h
    streamingintervaltype.h
    synchronizationerror.h
    synchronizationmanager.h
    synchronizationoptions.h
    valuerelationfeaturesmodel.h
    variablesmanager.h
    workspacesmodel.h
    workspacesproxymodel.h
)

if (HAVE_BLUETOOTH)
  set(MM_SRCS ${MM_SRCS} position/bluetoothpositionprovider.cpp)

  set(MM_HDRS ${MM_HDRS} position/bluetoothpositionprovider.h)
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
      test/testingpurchasingbackend.cpp
      test/testlayertree.cpp
      test/testlinks.cpp
      test/testmaptools.cpp
      test/testmerginapi.cpp
      test/testmodels.cpp
      test/testposition.cpp
      test/testpurchasing.cpp
      test/testrememberattributescontroller.cpp
      test/testscalebarkit.cpp
      test/testutils.cpp
      test/testutilsfunctions.cpp
      test/testvariablesmanager.cpp
      test/testactiveproject.cpp
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
      test/testingpurchasingbackend.h
      test/testlayertree.h
      test/testlinks.h
      test/testmaptools.h
      test/testmerginapi.h
      test/testmodels.h
      test/testposition.h
      test/testpurchasing.h
      test/testrememberattributescontroller.h
      test/testscalebarkit.h
      test/testutils.h
      test/testutilsfunctions.h
      test/testvariablesmanager.h
      test/testactiveproject.h
  )
endif ()

if (IOS)
  set(MM_HDRS ${MM_HDRS} ios/iosinterface.h ios/iosviewdelegate.h)

  set(MM_SRCS
      ${MM_SRCS}
      ios/iosinterface.mm
      ios/iosviewdelegate.mm
      ios/iosimagepicker.mm
      ios/iosutils.mm
  )
endif ()

if (HAVE_APPLE_PURCHASING)
  set(MM_HDRS ${MM_HDRS} ios/iospurchasing.h)

  set(MM_SRCS ${MM_SRCS} ios/iospurchasing.mm)
endif ()

qt_add_resources(
  MM_SRCS
  img/pics.qrc
  i18n/input_i18n.qrc
  fonts/fonts.qrc
)

# ########################################################################################
# Mergin Maps Target
# ########################################################################################

qt_add_executable(
  Input
  ${MM_SRCS}
  ${MM_HDRS}
  $<TARGET_OBJECTS:mm_core>
  main.cpp
)

target_include_directories(
  Input
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/attributes
         ${CMAKE_CURRENT_SOURCE_DIR}/map
         ${CMAKE_CURRENT_SOURCE_DIR}/layer
         ${CMAKE_CURRENT_SOURCE_DIR}/maptools
         ${CMAKE_CURRENT_SOURCE_DIR}/position
)

if (ANDROID)
  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml)
    file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml)
  endif ()
  configure_file(
    ${INPUT_CMAKE_TEMPLATES_PATH}/AndroidManifest.xml.in
    ${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml @ONLY
  )
  set_target_properties(
    Input
    PROPERTIES QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
               QT_ANDROID_TARGET_SDK_VERSION ${INPUT_ANDROID_TARGET_SDK_VERSION}
               QT_ANDROID_MIN_SDK_VERSION ${INPUT_ANDROID_MIN_SDK_VERSION}
               QT_ANDROID_VERSION_CODE ${INPUT_VERSION_CODE}
               QT_ANDROID_VERSION_NAME ${INPUT_VERSION}
  )
endif ()

if (MACOS)
  set_target_properties(
    Input
    PROPERTIES MACOSX_BUNDLE TRUE
               MACOSX_RPATH TRUE
               MACOSX_BUNDLE_GUI_IDENTIFIER LutraConsultingLtd.Input
               MACOSX_BUNDLE_INFO_PLIST ${INPUT_CMAKE_TEMPLATES_PATH}/iOSInfo.plist.in
               MACOSX_BUNDLE_SHORT_VERSION_STRING ${INPUT_VERSION_CODE}
               XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@loader_path/Libraries"
               XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME TRUE
  )
endif ()

if (IOS)
  # demo projects
  set(IOS_DEMO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/assets/demo-projects)
  target_sources(Input PRIVATE ${IOS_DEMO_DIR})
  set_source_files_properties(
    ${IOS_DEMO_DIR} PROPERTIES MACOSX_PACKAGE_LOCATION Resources
  )

  # qgis resources
  set(IOS_CRS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/assets/qgis-data/resources)
  target_sources(Input PRIVATE ${IOS_CRS_DIR})
  set_source_files_properties(
    ${IOS_CRS_DIR} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/qgis-data
  )

  # proj
  set(IOS_PROJ_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/assets/qgis-data/proj)
  target_sources(Input PRIVATE ${IOS_PROJ_DIR})
  set_source_files_properties(
    ${IOS_PROJ_DIR} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/qgis-data
  )

  # app icon
  set(IOS_ASSET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ios/Images.xcassets)
  target_sources(Input PRIVATE ${IOS_ASSET_DIR})
  set_source_files_properties(
    ${IOS_ASSET_DIR} PROPERTIES MACOSX_PACKAGE_LOCATION Resources
  )

  # launch screen
  set(IOS_LAUNCHSCREEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ios/launchscreen/InputScreen.xib)
  target_sources(Input PRIVATE ${IOS_LAUNCHSCREEN_DIR})
  set_source_files_properties(
    ${IOS_LAUNCHSCREEN_DIR} PROPERTIES MACOSX_PACKAGE_LOCATION Resources
  )

  set_target_properties(
    Input
    PROPERTIES MACOSX_BUNDLE TRUE
               MACOSX_BUNDLE_INFO_PLIST ${INPUT_CMAKE_TEMPLATES_PATH}/iOSInfo.plist.in
               MACOSX_BUNDLE_GUI_IDENTIFIER "LutraConsultingLtd.Input"
               MACOSX_BUNDLE_SHORT_VERSION_STRING ${INPUT_VERSION}
               MACOSX_BUNDLE_BUNDLE_VERSION ${INPUT_VERSION_CODE}
               XCODE_ATTRIBUTE_PROVISIONING_PROFILE_SPECIFIER
               "LutraConsultingLtd.Input.AppStore"
               XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "79QMH2QRAH"
               XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY
               "iPhone Distribution: LUTRA CONSULTING LIMITED (79QMH2QRAH)"
               XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual"
               XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon"
  )
endif ()

# ########################################################################################
# LINKING Mergin Maps Target
# ########################################################################################

# at least on Linux qgis_core needs to be linked before Qt libs, otherwise one gets "DSO
# missing from command line" linker error about missing symbol from qt6::core5compat
# library - probably if our code does not use anything from that library, it is not added
# (linking order matters)
target_link_libraries(Input PUBLIC QGIS::Core)

target_link_libraries(
  Input
  PUBLIC Qt6::Quick
         Qt6::Qml
         Qt6::Xml
         Qt6::Concurrent
         Qt6::Positioning
         Qt6::Sensors
         Qt6::QuickControls2
         Qt6::Network
         Qt6::Svg
         Qt6::Sql
         Qt6::Core
         Qt6::Core5Compat
)

if (HAVE_BLUETOOTH)
  target_link_libraries(Input PUBLIC Qt6::Bluetooth)
endif ()

target_link_libraries(Input PUBLIC Qt6::Multimedia Qt6::Widgets)
if (NOT IOS)
  target_link_libraries(Input PUBLIC Qt6::PrintSupport)
endif ()

if (ANDROID)
  target_link_libraries(Input PUBLIC Qt6::CorePrivate)
endif ()

target_link_libraries(
  Input
  PUBLIC mm_core
         Geodiff::Geodiff
         Proj::Proj
         ZXing::ZXing
)

target_link_libraries(
  Input
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
  target_link_libraries(Input PUBLIC QGIS::Core)
endif ()

target_link_libraries(
  Input
  PUBLIC Qtkeychain::Qtkeychain
         Qca::Qca
         Gdal::Gdal
         Pq::Pq
         Spatialite::Spatialite
)

if (MACOS
    OR IOS
    OR ANDROID
)
  target_link_libraries(Input PUBLIC Charset::Charset Iconv::Iconv)
  target_link_libraries(Input PUBLIC Pq::Pgcommon Pq::Pgport)
endif ()

if (MACOS OR IOS)
  target_link_libraries(Input PUBLIC Bz2::Bz2)
endif ()

if (NOT ANDROID)
  target_link_libraries(Input PUBLIC Xml2::Xml2)
endif ()

target_link_libraries(
  Input
  PUBLIC Spatialindex::Spatialindex
         Geos::Geos
         Geos::Geos-c
         Protobuf::Protobuf-lite
         Expat::Expat
         Freexl::Freexl
         Exiv2::Exiv2
         Exiv2::Exiv2-xmp
         Tiff::Tiff
         Sqlite3::Sqlite3
         ZLIB::ZLIB
         Zip::Zip
         WebP::WebP
         Curl::Curl
         Jpeg::Jpeg
)

if (ANDROID
    OR IOS
    OR MACOS
)
  target_link_libraries(Input PUBLIC Tasn1::Tasn1)
endif ()

if (ANDROID)
  target_link_libraries(Input PUBLIC OpenSSL::OpenSSL OpenSSL::Crypto)
  set_property(
    TARGET Input
    PROPERTY QT_ANDROID_EXTRA_LIBS ${INPUT_SDK_PATH_MULTI}/lib/libcrypto_3.so
             ${INPUT_SDK_PATH_MULTI}/lib/libssl_3.so
  )
endif ()

if (MACOS OR IOS)
  target_link_libraries(
    Input PUBLIC AppleFrameworks::Security AppleFrameworks::CoreFoundation
                 AppleFrameworks::SystemConfiguration
  )
endif ()

if (HAVE_APPLE_PURCHASING)
  target_link_libraries(
    Input PUBLIC AppleFrameworks::StoreKit AppleFrameworks::Foundation
  )
endif ()

if (ENABLE_TESTS)
  target_link_libraries(Input PUBLIC Qt6::Test)
endif ()

# ########################################################################################
# QML
# ########################################################################################

add_subdirectory(qml)

# register Singleton
set_source_files_properties(qml/InputStyle.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
set_source_files_properties(qml/MMCamera.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
set_source_files_properties(qml/Propagation.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

# make sure the QML files are not prefixed with "qml/" in the qmldir
set(MM_QML_SRCS)
foreach (src ${MM_QML})
  set(MM_QML_SRCS ${MM_QML_SRCS} qml/${src})
  set_source_files_properties(qml/${src} PROPERTIES QT_RESOURCE_ALIAS ${src})
endforeach ()

# For some reason, this call cannot be in qml/CMakeLists.txt
qt_add_qml_module(
  Input
  URI MMInput
  VERSION 1.0
  QML_FILES ${MM_QML_SRCS}
  RESOURCE_PREFIX /com.merginmaps/imports
)
