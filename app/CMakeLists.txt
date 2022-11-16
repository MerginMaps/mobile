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
    synchronizationerror.cpp
    synchronizationmanager.cpp
    valuerelationfeaturesmodel.cpp
    variablesmanager.cpp
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
    synchronizationerror.h
    synchronizationmanager.h
    synchronizationoptions.h
    valuerelationfeaturesmodel.h
    variablesmanager.h
)

if (HAVE_BLUETOOTH)
    set(MM_SRCS ${MM_SRCS} 
        position/bluetoothpositionprovider.cpp
    )
    
    set(MM_HDRS ${MM_HDRS} 
        position/bluetoothpositionprovider.h
    )
    
endif()

if (ENABLE_TESTS)
    set(MM_SRCS ${MM_SRCS} 
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
    )
    
    set(MM_HDRS ${MM_HDRS} 
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
    )
endif()

qt_add_resources(MM_SRCS 
  img/pics.qrc 
  i18n/input_i18n.qrc 
  fonts/fonts.qrc
)

qt_add_executable(merginmaps MANUAL_FINALIZATION ${MM_SRCS} ${MM_HDRS} $<TARGET_OBJECTS:mm_core> main.cpp)

if (APPLE)
  set_target_properties(merginmaps PROPERTIES
    OUTPUT_NAME "Mergin Maps"
    MACOSX_BUNDLE TRUE
    MACOSX_RPATH TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER com.merginmaps.input
    # XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@loader_path/Libraries"
    # RESOURCE "${RESOURCE_FILES}"
    XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME TRUE
    XCODE_ATTRIBUTE_EXECUTABLE_NAME "merginmaps"
  )
endif()

target_include_directories(merginmaps PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/attributes ${CMAKE_CURRENT_SOURCE_DIR}/map ${CMAKE_CURRENT_SOURCE_DIR}/layer ${CMAKE_CURRENT_SOURCE_DIR}/maptools ${CMAKE_CURRENT_SOURCE_DIR}/position)
target_link_libraries(merginmaps PUBLIC Qt6::Quick Qt6::Qml Qt6::Xml Qt6::Concurrent Qt6::Positioning Qt6::Sensors Qt6::QuickControls2 Qt6::Network Qt6::Svg Qt6::Sql Qt6::Core Qt6::Core5Compat) # TODO: OpenGl?
target_link_libraries(merginmaps PUBLIC Qt6::Bluetooth Qt6::Multimedia Qt6::Widgets)
if (NOT IOS)
  # https://doc.qt.io/qt-6/qtprintsupport-index.html
  # Qt Print Support is not available on iOS.
  target_link_libraries(merginmaps PUBLIC  Qt6::PrintSupport)
endif()

target_link_libraries(merginmaps PUBLIC
  mm_core 
  QGIS::Core 
  Geodiff::Geodiff 
  Proj::Proj
  ZXing::ZXing
  QGIS::authmethod_basic
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
  Qtkeychain::Qtkeychain
  Qca::Qca
  Gdal::Gdal
  Pq::Pq
  Spatialite::Spatialite
  Charset::Charset
  Xml2::Xml2 
  Tasn1::Tasn1
  Bz2::Bz2
  Proj::Proj # linking 2x why (macos?)
  Spatialindex::Spatialindex
  Geos::Geos Geos::Geos-c
  Protobuf::Protobuf-lite 
  Expat::Expat
  Freexl::Freexl
  Exiv2::Exiv2 Exiv2::Exiv2-xmp
  Tiff::Tiff
  Sqlite3::Sqlite3
  Iconv::Iconv
  ZLIB::ZLIB
  Zip::Zip
  WebP::WebP
  Curl::Curl
  Jpeg::Jpeg
)

if(APPLE)
  target_link_libraries(merginmaps PUBLIC 
      AppleFrameworks::Security
      AppleFrameworks::CoreFoundation
      AppleFrameworks::SystemConfiguration
  )
endif()

if (ENABLE_TESTS)
    target_link_libraries(merginmaps PUBLIC Qt6::Test)
endif()

add_subdirectory(qml)

set_source_files_properties(qml/InputStyle.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(merginmaps
    URI MMInput
    VERSION 1.0
    QML_FILES ${MM_QML}
    RESOURCE_PREFIX /com.merginmaps/imports
)

qt_finalize_target(merginmaps)