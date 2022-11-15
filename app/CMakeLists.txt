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

set(MM_QML
    qml/banners/AutoHideBanner.qml
    
    qml/components/BluetoothConnectionDialog.qml
    qml/components/CheckboxComponent.qml
    qml/components/DateTimePicker.qml
    qml/components/DelegateButton.qml
    qml/components/ExtendedMenuItem.qml
    qml/components/FieldRow.qml
    qml/components/IconTextItem.qml
    qml/components/InputComboBox.qml
    qml/components/InputText.qml
    qml/components/InputTextField.qml
    qml/components/LayerList.qml
    qml/components/LeftCheckBox.qml
    qml/components/LoadingSpinner.qml
    qml/components/MainPanelButton.qml
    qml/components/MapFloatButton.qml
    qml/components/NumberInputField.qml
    qml/components/PanelHeader.qml
    qml/components/PanelHeaderV2.qml
    qml/components/PasswordField.qml
    qml/components/ProjectDelegateItem.qml
    qml/components/ProjectList.qml
    qml/components/RichTextBlock.qml
    qml/components/SearchBoxV2.qml
    qml/components/SettingsSwitch.qml
    qml/components/Shadow.qml
    qml/components/SimpleTextWithIcon.qml
    qml/components/Switch.qml
    qml/components/Symbol.qml
    qml/components/TextHyperlink.qml
    qml/components/TextRowWithTitle.qml
    qml/components/TextWithIcon.qml
    qml/components/ToolbarButton.qml
    
    qml/dialogs/MigrateToMerginDialog.qml
    qml/dialogs/MissingAuthDialog.qml
    qml/dialogs/NoPermissionsDialog.qml
    qml/dialogs/SplittingFailedDialog.qml
    qml/dialogs/SyncFailedDialog.qml
    
    qml/editor/AbstractEditor.qml
    qml/editor/RelationPhotoDelegate.qml
    qml/editor/RelationPhotoFooterDelegate.qml
    qml/editor/RelationTextDelegate.qml
    qml/editor/inputcheckbox.qml
    qml/editor/inputdatetime.qml
    qml/editor/inputexternalresource.qml
    qml/editor/inputqrcodereader.qml
    qml/editor/inputrangeeditable.qml
    qml/editor/inputrangeslider.qml
    qml/editor/inputrelation.qml
    qml/editor/inputrelationreference.qml
    qml/editor/inputtextedit.qml
    qml/editor/inputtexteditmultiline.qml
    qml/editor/inputvaluemap.qml
    qml/editor/inputvaluerelationcombobox.qml
    qml/editor/inputvaluerelationpage.qml
    
    qml/form/FeatureForm.qml
    qml/form/FeatureFormPage.qml
    qml/form/FeatureFormStyling.qml
    qml/form/FeatureToolbar.qml
    qml/form/FormWrapper.qml
    qml/form/PreviewPanel.qml
    
    qml/layers/FeaturesListPageV2.qml
    qml/layers/LayerDetail.qml
    qml/layers/LayersListPageV2.qml
    qml/layers/LayersListSearchPage.qml
    qml/layers/LayersListV2.qml
    qml/layers/LayersPanelV2.qml
    
    qml/map/ActiveLayerPanel.qml
    qml/map/Crosshair.qml
    qml/map/Highlight.qml
    qml/map/LoadingIndicator.qml
    qml/map/MapCanvas.qml
    qml/map/MapWrapper.qml
    qml/map/PositionMarker.qml
    qml/map/RecordingToolbar.qml
    qml/map/RecordingTools.qml
    qml/map/ScaleBar.qml
    qml/map/SplittingToolbar.qml
    qml/map/SplittingTools.qml
    qml/map/StakeoutTools.qml
    
    qml/misc/AddPositionProviderPage.qml
    qml/misc/AttentionBanner.qml
    qml/misc/GpsDataPage.qml
    qml/misc/LegacyFolderMigration.qml
    qml/misc/PositionProviderPage.qml
    
    qml/AboutPanel.qml
    qml/AccountPage.qml
    qml/AuthPanel.qml
    qml/Banner.qml
    qml/CircularProgressBar.qml
    qml/CodeReaderHandler.qml
    qml/CodeScanner.qml
    qml/CodeScannerOverlay.qml
    qml/ExternalResourceBundle.qml
    qml/FeaturesList.qml
    qml/FeaturesListPage.qml
    qml/FeaturesListPageToolbar.qml
    qml/FormsStackManager.qml
    qml/InputStyle.qml
    qml/LogPanel.qml
    qml/LoginForm.qml
    qml/MainPanel.qml
    qml/MapThemePanel.qml
    qml/Notification.qml
    qml/NotificationBanner.qml
    qml/NumberSpin.qml
    qml/PanelItem.qml
    qml/PanelTabButton.qml
    qml/PhotoPanel.qml
    qml/ProjectIssuesPanel.qml
    qml/ProjectListPage.qml
    qml/ProjectLoadingScreen.qml
    qml/ProjectPanel.qml
    qml/ProjectStatusPanel.qml
    qml/ProjectWizardPage.qml
    qml/RegistrationForm.qml
    qml/RoundIndicator.qml
    qml/SearchBar.qml
    qml/SettingsPanel.qml
    qml/StakeoutPanel.qml
    qml/StorageLimitDialog.qml
    qml/SubscribePage.qml
    qml/SubscribePlanItem.qml
    qml/main.qml
)

set (MM_PNGS
    img/logo.png
    img/mIconLayer.png
    img/stakeout.png
)

set (MM_SVGS  
    img/account-multi.svg
    img/account.svg
    img/add_photo.svg
    img/arrows-move.svg
    img/back.svg
    img/backv2.svg
    img/baseline-edit-24px.svg
    img/baseline-gps_off-24px.svg
    img/check.svg
    img/cloud-upload.svg
    img/combobox.svg
    img/crosshair-background.svg
    img/crosshair-center.svg
    img/crosshair-circle.svg
    img/crosshair-foreground.svg
    img/crosshair-plus.svg
    img/database-solid.svg
    img/dot-circle.svg
    img/download.svg
    img/edit.svg
    img/envelope-solid.svg
    img/eraser.svg
    img/exclamation-circle.svg
    img/exclamation-triangle-solid.svg
    img/explore.svg
    img/eye-slash.svg
    img/eye.svg
    img/eye_slashv2.svg
    img/eyev2.svg
    img/file.svg
    img/file_info.svg
    img/file_info_inactive.svg
    img/gallery.svg
    img/gps_direction.svg
    img/gps_marker_no_position.svg
    img/gps_marker_position.svg
    img/home.svg
    # img/ic_angle_down 2.svg
    img/ic_angle_down.svg
    # img/ic_angle_right 2.svg
    img/ic_angle_right.svg
    # img/ic_back 2.svg
    img/ic_back.svg
    # img/ic_camera 2.svg
    img/ic_camera.svg
    # img/ic_camera_alt_border 2.svg
    img/ic_camera_alt_border.svg
    # img/ic_check_black 2.svg
    img/ic_check_black.svg
    # img/ic_clear_black 2.svg
    img/ic_clear_black.svg
    # img/ic_clear_white 2.svg
    img/ic_clear_white.svg
    # img/ic_delete_forever_white 2.svg
    img/ic_delete_forever_white.svg
    # img/ic_gallery 2.svg
    img/ic_gallery.svg
    img/ic_gps_fixed_48px.svg
    img/ic_gps_not_fixed_48px.svg
    # img/ic_navigation_black 2.svg
    img/ic_navigation_black.svg
    # img/ic_save_white 2.svg
    img/ic_save_white.svg
    img/ic_today.svg
    img/info.svg
    img/link-rotated.svg
    img/link.svg
    img/loadingindicator.svg
    img/lock.svg
    img/lutra_logo.svg
    img/mActionFolder.svg
    img/mIconAnnotationLayer.svg
    img/mIconGeometryCollectionLayer.svg
    img/mIconGroup.svg
    img/mIconLineLayer.svg
    img/mIconMeshLayer.svg
    img/mIconPointCloudLayer.svg
    img/mIconPointLayer.svg
    img/mIconPolygonLayer.svg
    img/mIconRaster.svg
    img/mIconRasterLayer.svg
    img/mIconTableLayer.svg
    img/mIconVectorTileLayer.svg
    img/mIndicatorBadLayer.svg
    img/map_search.svg
    img/map_styles.svg
    img/marker.svg
    img/mergin.svg
    img/mergin_color.svg
    img/minus.svg
    img/mm_logo_hor_pos.svg
    img/mm_logo_vert_pos.svg
    img/more_menu.svg
    img/no-image.svg
    img/no.svg
    img/plus-big.svg
    img/plus.svg
    img/plusv2.svg
    img/project.svg
    img/qrcode.svg
    img/scissors.svg
    img/search.svg
    img/searchv2.svg
    img/settings.svg
    img/stop.svg
    img/sync.svg
    img/table.svg
    img/table_inactivev2.svg
    img/tablev2.svg
    img/trash.svg
    img/undo.svg
    img/unlink.svg
    img/value_relation_open.svg
    img/yes.svg
    img/zoom_to_project.svg
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

qt_add_executable(merginmaps ${MM_SRCS} ${MM_HDRS} $<TARGET_OBJECTS:mm_core> main.cpp)
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

qt_add_qml_module(merginmaps
    URI MMInput
    VERSION 1.0
    QML_FILES ${MM_QML}
    RESOURCE_PREFIX /com.merginmaps/imports
    RESOURCES ${MM_PNGS} ${MM_SVGS}
)