/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.2

// Required for iOS to get rid of "module "QtMultimedia" is not installed".
// It looks like static QT plugins are not copied to the distribution
import QtMultimedia 5.8
import QtQml.Models 2.2
import QtPositioning 5.8
import QtQuick.Dialogs 1.1
import QgsQuick 0.1 as QgsQuick
import lc 1.0

ApplicationWindow {
    id: window
    visible: true
    width:  __appwindowwidth
    height: __appwindowheight
    visibility: __appwindowvisibility
    title: "Input" // Do not translate

    property int zMapCanvas: 0
    property int zPanel: 20
    property int zToolkits: 10

    Item {
        id: stateManager
        state: "view"
        states: [
            // Default state - when none of state below
            State {
                name: "view"
            },
            // When a user is in recording session - creating a new feature.
            State {
                name: "record"
            },
            // When a user is modifying geometry of an existing feature
            State {
                name: "edit"
            }
        ]

        onStateChanged: {
            if (stateManager.state === "view") {
                recordToolbar.visible = false
                mainPanel.focus = true
                digitizing.stopRecording();
            }
            else if (stateManager.state === "record") {
                updateRecordToolbar()
                recordToolbar.visible = true
                recordToolbar.focus = true
                recordToolbar.extraPanelVisible = true
                recordToolbar.gpsSwitchClicked()
                digitizing.layer = recordToolbar.activeVectorLayer
            }
            else if (stateManager.state === "edit") {
                recordToolbar.focus = true
                recordToolbar.visible = true
                recordToolbar.extraPanelVisible = false

                let pair = formsStackManager.getFeaturePair()
                __loader.setActiveLayer( pair.layer )
                updateRecordToolbar()

                var screenPos = digitizing.pointFeatureMapCoordinates( pair )
                mapCanvas.mapSettings.setCenter(screenPos);

                browseDataPanel.clearStackAndClose()
            }
        }
    }

    function isPositionOutOfExtent(border) {
        return ((positionKit.screenPosition.x < border) ||
                (positionKit.screenPosition.y < border) ||
                (positionKit.screenPosition.x > mapCanvas.width -  border) ||
                (positionKit.screenPosition.y > mapCanvas.height -  border)
                )
    }

    function saveRecordedFeature( pair, hasGeometry = true ) {

      if ( !digitizing.isPairValid( pair ) && hasGeometry ||
            !pair.layer && !hasGeometry ) {
        popup.text = qsTr( "Recorded feature is not valid" )
        popup.open()
      }
      else {
        if ( hasGeometry ) {
          digitizingHighlight.featureLayerPair = pair
          digitizingHighlight.visible = true
        }

        formsStackManager.openForm( pair, "Add", "form" )
      }

      stateManager.state = "view"
    }


    //! Returns point from gps (WGS84) or center screen point in map CRS
    function getRecordedPoint() {
      if (digitizing.useGpsPoint) {
         return positionKit.position  // WGS84
      } else {
        var screenPoint = Qt.point( mapCanvas.width/2, mapCanvas.height/2 )
        return mapCanvas.mapSettings.screenToCoordinate(screenPoint)  // map CRS
      }
    }

    function editFeature() {
        var pair = formsStackManager.getFeaturePair()
        if (!pair || !pair.layer)
        {
            // nothing to do with no active layer
            return
        }

        let layer = pair.layer
        if (digitizing.hasLineGeometry(layer)) {
            // TODO
        }
        else if (digitizing.hasPointGeometry(layer)) {
            var recordedPoint = getRecordedPoint()
            var newFormState = "Edit"
            let featurePair = digitizing.changePointGeometry(formsStackManager.getFeaturePair(), recordedPoint, digitizing.useGpsPoint)
            formsStackManager.setFeaturePair( featurePair )

            if ( formsStackManager.isNewFeature() ) {
              digitizingHighlight.featureLayerPair = featurePair
              digitizingHighlight.visible = true
              newFormState = "Add"
            }
            else {
              // save only existing feature
              formsStackManager.updateFeatureGeometry()
            }

            stateManager.state = "view"
            formsStackManager.geometryEditingFinished( newFormState )
        }
    }

    function recordFeature( hasGeometry = true ) {
      if ( hasGeometry )
      {
        var recordedPoint = getRecordedPoint()

        if ( digitizing.hasPointGeometry( __activeLayer.layer ) ) {
          var pair = digitizing.pointFeatureFromPoint( recordedPoint, digitizing.useGpsPoint )
          saveRecordedFeature( pair )
        }
        else {
          if ( !digitizing.recording )
            digitizing.startRecording()

          digitizing.addRecordPoint( recordedPoint, digitizing.useGpsPoint )
        }
      }
      else
      {
        saveRecordedFeature( digitizing.featureWithoutGeometry(), hasGeometry )
      }
    }

    function isGpsAccuracyLow() {
      return (positionKit.accuracy <= 0) || (positionKit.accuracy > __appSettings.gpsAccuracyTolerance)
    }

    function getGpsIndicatorColor() {
        if (positionKit.accuracy <= 0) return InputStyle.softRed
        return isGpsAccuracyLow() ? InputStyle.softOrange : InputStyle.softGreen
    }

    function showGpsAccuracyWarning() {
      return (__appSettings.gpsAccuracyWarning && positionKit.hasPosition && (digitizing.useGpsPoint || !digitizing.manualRecording) && isGpsAccuracyLow() )
    }

    function showMessage(message) {
        if ( !__androidUtils.isAndroid ) {
            popup.text = message
            popup.open()
        } else {
            __androidUtils.showToast( message )
        }
    }

    function showDialog(message) {
      alertDialog.text  = message
      alertDialog.open()
    }

    function showProjError(message) {
      projDialog.text  = message
      projDialog.open()
    }

    function updateRecordToolbar()
    {
      if ( !__activeLayer.layer )
        __loader.setActiveLayer( __recordingLayersModel.firstUsableLayer() )

      recordToolbar.activeVectorLayer = __activeLayer.vectorLayer
      digitizing.layer = recordToolbar.activeVectorLayer
      
      if ( !recordToolbar.activeVectorLayer ) // nothing to do with no active layer
        return

      recordToolbar.pointLayerSelected = digitizing.hasPointGeometry( recordToolbar.activeVectorLayer )
    }

    function updateBrowseDataPanel()
    {
      if ( browseDataPanel.visible )
        browseDataPanel.refreshFeaturesData()
    }

    function selectFeature( feature, shouldUpdateExtent, hasGeometry = true ) {

      // update extent to fit feature above preview panel
      if ( shouldUpdateExtent ) {
          let panelOffsetRatio = formsStackManager.previewHeight/window.height
          __inputUtils.setExtentToFeature( feature, mapCanvas.mapSettings, panelOffsetRatio )
      }

      if ( hasGeometry ) {
        highlight.featureLayerPair = feature
        highlight.visible = true
        formsStackManager.openForm( feature, "ReadOnly", "preview" )
      }
      else
        formsStackManager.openForm( feature, "ReadOnly", "form" )
    }

    function updatePosition() {
      if ((digitizing.useGpsPoint && stateManager.state !== "view")|| (stateManager.state === "view" && __appSettings.autoCenterMapChecked && isPositionOutOfExtent(mainPanel.height))) {
        var useGpsPoint = digitizing.useGpsPoint
        mapCanvas.mapSettings.setCenter(positionKit.projectedPosition);
        // sets previous useGpsPoint value, because setCenter triggers extentChanged signal which changes this property
        digitizing.useGpsPoint = useGpsPoint
      }

      digitizingHighlight.positionChanged()
    }

    Component.onCompleted: {
      // load default project
      if ( __appSettings.defaultProject ) {
        let path = __appSettings.defaultProject

        if ( __localProjectsManager.projectIsValid( path ) && __loader.load( path ) ) {
          projectPanel.activeProjectPath = path
          projectPanel.activeProjectId = __localProjectsManager.projectId( path )
          __appSettings.activeProject = path
        }
        else {
          // if default project load failed, delete default setting
          __appSettings.defaultProject = ""
          projectPanel.openPanel()
        }
      }
      else projectPanel.openPanel()

        InputStyle.deviceRatio = window.screen.devicePixelRatio
        InputStyle.realWidth = window.width
        InputStyle.realHeight = window.height

        __loader.positionKit = positionKit
        __loader.recording = digitizing.recording
        __loader.mapSettings = mapCanvas.mapSettings
        __iosUtils.positionKit = positionKit
        __iosUtils.compass = compass
        __variablesManager.compass = compass
        __variablesManager.positionKit = positionKit

        // get focus when any project is active, otherwise let focus to merginprojectpanel
        if ( __appSettings.activeProject )
          mainPanel.forceActiveFocus()

        console.log("Completed Running!")
    }

    QgsQuick.MapCanvas {
      id: mapCanvas

      height: parent.height - mainPanel.height
      width: parent.width
      z: zMapCanvas

      mapSettings.project: __loader.project

      IdentifyKit {
        id: identifyKit
        mapSettings: mapCanvas.mapSettings
        identifyMode: IdentifyKit.TopDownAll
      }

      onIsRenderingChanged: {
        loadingIndicator.visible = isRendering
      }

      onClicked: {
       // no identify action in record state
       if (stateManager.state === "record") return

        mapCanvas.forceActiveFocus()
        var screenPoint = Qt.point( mouse.x, mouse.y );
        var res = identifyKit.identifyOne(screenPoint);

        if (res.valid) {
          let shouldUpdateExtent = mouse.y > window.height - formsStackManager.previewHeight
          selectFeature( res, shouldUpdateExtent )
        } else { // closes feature/preview panel when there is nothing to show
          formsStackManager.closeDrawer()
        }
      }
    }

    Highlight {
        id: highlight
        anchors.fill: mapCanvas

        property bool hasPolygon: featureLayerPair !== null ? digitizing.hasPolygonGeometry(featureLayerPair.layer) : false

        mapSettings: mapCanvas.mapSettings

        lineColor: Qt.rgba(1,0.2,0.2,1)
        lineWidth: 6 * QgsQuick.Utils.dp

        fillColor: Qt.rgba(1,0.2,0.2,InputStyle.lowHighlightOpacity)

        outlinePenWidth: 1 * QgsQuick.Utils.dp
        outlineColor: "white"

        markerType: "image"
        markerImageSource: "qrc:/marker.svg"
        markerWidth: 60 * QgsQuick.Utils.dp
        markerHeight: 70 * QgsQuick.Utils.dp
        markerAnchorY: 48 * QgsQuick.Utils.dp

        // enable anti-aliasing to make the higlight look nicer
        // https://stackoverflow.com/questions/48895449/how-do-i-enable-antialiasing-on-qml-shapes
        layer.enabled: true
        layer.samples: 4
    }

    Highlight {
      id: digitizingHighlight
      anchors.fill: mapCanvas

      hasPolygon: featureLayerPair !== null ? digitizing.hasPolygonGeometry(featureLayerPair.layer) : false

      mapSettings: mapCanvas.mapSettings

      lineColor: highlight.lineColor
      lineWidth: highlight.lineWidth

      fillColor: highlight.fillColor

      outlinePenWidth: highlight.outlinePenWidth
      outlineColor: highlight.outlineColor

      markerType: highlight.markerType
      markerImageSource: highlight.markerImageSource
      markerWidth: highlight.markerWidth
      markerHeight: highlight.markerHeight
      markerAnchorY: highlight.markerAnchorY
      recordingInProgress: digitizing.recording
      guideLineAllowed: digitizing.manualRecording && stateManager.state === "record"

      // enable anti-aliasing to make the higlight look nicer
      // https://stackoverflow.com/questions/48895449/how-do-i-enable-antialiasing-on-qml-shapes
      layer.enabled: true
      layer.samples: 4
    }

    Item {
        anchors.fill: mapCanvas
        transform: QgsQuick.MapTransform {
            mapSettings: mapCanvas.mapSettings
        }
        z: zMapCanvas + 1  // make sure items from here are on top of the Z-order
    }

    SettingsPanel {
      id: settingsPanel
      height: window.height
      width: window.width
      rowHeight: InputStyle.rowHeight
      z: zPanel   // make sure items from here are on top of the Z-order

      onVisibleChanged: {
        if (settingsPanel.visible)
          settingsPanel.focus = true; // get focus
        else
          mainPanel.focus = true; // pass focus back to main panel
      }

      gpsIndicatorColor: getGpsIndicatorColor()
    }
	
    // Position Kit and Marker
    PositionKit {
      id: positionKit
      mapSettings: mapCanvas.mapSettings
      simulatePositionLongLatRad: __use_simulated_position ? [-2.9207148, 51.3624998, 0.05] : []
      onScreenPositionChanged: updatePosition()
    }

    Compass {
      id: compass
    }

    PositionMarker {
      id: positionMarker
      positionKit: positionKit
      compass: compass
      z: zMapCanvas + 2
    }

    DigitizingController {
        id: digitizing
        positionKit: positionMarker.positionKit
        layer: recordToolbar.activeVectorLayer
        lineRecordingInterval: __appSettings.lineRecordingInterval
        mapSettings: mapCanvas.mapSettings
        variablesManager: __variablesManager

        onRecordingChanged: {
            __loader.recording = digitizing.recording
        }

        onFeatureLayerPairChanged: {
            if (digitizing.recording) {
                digitizingHighlight.visible = true
                digitizingHighlight.featureLayerPair = digitizing.featureLayerPair
            }
        }

        onUseGpsPointChanged: __variablesManager.useGpsPoint = digitizing.useGpsPoint
    }

    MainPanel {
        id: mainPanel
        width: window.width
        height: InputStyle.rowHeightHeader
        z: zToolkits + 1
        y: window.height - height

        gpsIndicatorColor: getGpsIndicatorColor()

        onOpenProjectClicked: projectPanel.openPanel()
        onOpenMapThemesClicked: mapThemesPanel.visible = true
        onMyLocationClicked: {
          mapCanvas.mapSettings.setCenter(positionKit.projectedPosition)
          digitizing.useGpsPoint = true
        }
        onMyLocationHold: {
            __appSettings.autoCenterMapChecked =!__appSettings.autoCenterMapChecked
            showMessage(__appSettings.autoCenterMapChecked ?  qsTr("GPS auto-center mode on") : qsTr("GPS auto-center mode off"))
        }
        onOpenSettingsClicked: settingsPanel.visible = true
        onZoomToProject: {
          if (__appSettings.autoCenterMapChecked) {
            mainPanel.myLocationHold()
          }
          __loader.zoomToProject(mapCanvas.mapSettings)
        }
        onOpenBrowseDataClicked: browseDataPanel.visible = true

        recordButton.recording: digitizing.recording
        onAddFeatureClicked: {
            if ( __recordingLayersModel.rowCount() > 0 ) {
                stateManager.state = "record"
            } else {
                showMessage(qsTr("No editable layers found."))
            }
        }
    }

    Connections {
      target: __activeLayer
      onActiveLayerChanged: {
        updateRecordToolbar()
      }
    }

    Banner {
      id: gpsAccuracyBanner
      width: parent.width - gpsAccuracyBanner.anchors.margins * 2
      height: InputStyle.rowHeight * 2
      text: qsTr("Low GPS position accuracy (%1 m)<br><br>Please make sure you have good view of the sky.")
        .arg(__inputUtils.formatNumber(positionKit.accuracy))
      link: "https://help.inputapp.io/howto/gps_accuracy"
      showWarning: recordToolbar.visible && showGpsAccuracyWarning()
    }

    RecordToolbar {
        id: recordToolbar
        width: window.width
        height: InputStyle.rowHeightHeader + ((extraPanelVisible) ? extraPanelHeight : 0)
        z: zToolkits + 1
        y: window.height - height
        visible: false
        gpsIndicatorColor: getGpsIndicatorColor()
        manualRecordig: digitizing.manualRecording
        // reset manualRecording after opening
        onVisibleChanged: {
          if (visible) digitizing.manualRecording = true
          if  (gpsAccuracyBanner.showWarning) {
            gpsAccuracyBanner.state = visible ? "show" : "fade"
          }
        }

        onAddClicked: {
            if (stateManager.state === "record") {
                recordFeature()
            } else if (stateManager.state === "edit") {
                editFeature()
            }
        }

        onGpsSwitchClicked: {
            if (!positionKit.hasPosition) {
                showMessage(qsTr("GPS currently unavailable.%1Try to allow GPS Location in your device settings.").arg("\n"))
                return // leaving when no gps is available
            }
            mapCanvas.mapSettings.setCenter(positionKit.projectedPosition)
            digitizing.useGpsPoint = true
        }

        onManualRecordingClicked: {
          digitizing.manualRecording = !digitizing.manualRecording
          if (!digitizing.manualRecording && stateManager.state === "record") {
            digitizing.startRecording()
            digitizing.useGpsPoint = true
            updatePosition()
            recordFeature() // record point immediately after turning on the streaming mode
          }
        }

        onCancelClicked: {
            if (stateManager.state === "edit") {
              formsStackManager.geometryEditingFinished( "Edit" )
            }
            stateManager.state = "view"
            digitizingHighlight.visible = false
        }

        onRemovePointClicked: {
            digitizing.removeLastPoint()
        }

         onStopRecordingClicked: {
             var pair = digitizing.lineOrPolygonFeature();
             saveRecordedFeature(pair)
             stateManager.state = "view"
         }

         onLayerLabelClicked: {
             if (!digitizing.recording) {
                 activeLayerPanel.openPanel()
             }
         }
    }

    RecordCrosshair {
        id: crosshair
        width: mapCanvas.width
        height: mapCanvas.height
        visible: recordToolbar.visible && digitizing.manualRecording
        z: positionMarker.z + 1
    }

    ScaleBar {
        id: scaleBar
        height: InputStyle.scaleBarHeight
        y: window.height - height - mainPanel.height - InputStyle.panelMargin
        mapSettings: mapCanvas.mapSettings
        preferredWidth: Math.min(window.width, 180 * QgsQuick.Utils.dp)
        z: zToolkits
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Connections {
        target: mapCanvas.mapSettings
        onExtentChanged: {
            digitizing.useGpsPoint = false
            scaleBar.visible = true
        }
    }

    ProjectPanel {
        id: projectPanel
        height: window.height
        width: window.width
        z: zPanel

        onVisibleChanged: {
          if (projectPanel.visible)
            projectPanel.forceActiveFocus()
          else
          {
            mainPanel.forceActiveFocus()
          }
        }

        onOpenProjectRequested: {
          __appSettings.defaultProject = projectPath
          __appSettings.activeProject = projectPath
          __loader.load( projectPath )
        }
    }

    ActiveLayerPanel {
        id: activeLayerPanel
        height: window.height/2
        width: window.width
        edge: Qt.BottomEdge
        z: zPanel

        onActiveLayerChangeRequested: {
          __loader.setActiveLayer( __recordingLayersModel.layerFromLayerId( layerId ) )
        }
    }

    BrowseDataPanel {
      id: browseDataPanel
      width: window.width
      height: window.height
      focus: true
      z: zPanel   // make sure items from here are on top of the Z-order

      onFeatureSelectRequested: {
        if ( pair.valid )
          selectFeature( pair, true )
        else if ( pair.feature.geometry.isNull )
          selectFeature( pair, false, false )
      }

      onCreateFeatureRequested: {
        digitizing.layer = selectedLayer
        recordFeature( false )
      }

      onVisibleChanged: {
        if ( !browseDataPanel.visible )
          mainPanel.forceActiveFocus()
      }
    }

    MapThemePanel {
        id: mapThemesPanel
        height: window.height/2
        width: window.width
        edge: Qt.BottomEdge
        z: zPanel
    }

    Notification {
        id: popup
        text: ""
        width: 400 * QgsQuick.Utils.dp
        height: 160 * QgsQuick.Utils.dp
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        z: zPanel + 1000 // the most top
    }

    StorageLimitDialog {
        id: storageLimitDialog
        onOpenSubscriptionPlans: {
          storageLimitDialog.close()
          if (__merginApi.apiSupportsSubscriptions) {
            projectPanel.manageSubscriptionPlans()
          }
        }
    }

    MessageDialog {
        id: alertDialog
        onAccepted: alertDialog.close()
        title: qsTr("Communication error")
    }

    MessageDialog {
        id: projDialog
        onAccepted: projDialog.close()
        title: qsTr("PROJ Error")
        standardButtons: StandardButton.Ignore |StandardButton.Help
        onHelp: Qt.openUrlExternally(__inputHelp.howToSetupProj)
    }

    Connections {
        target: __merginApi
        onNetworkErrorOccurred: {
            var msg = message ? message : qsTr("Failed to communicate with Mergin.%1Try improving your network connection.".arg("<br/>"))
            showAsDialog ? showDialog(msg) : showMessage(msg)
        }

        onStorageLimitReached: {
          __merginApi.getUserInfo()
          if (__merginApi.apiSupportsSubscriptions) {
            __merginApi.getSubscriptionInfo()
          }
          storageLimitDialog.uploadSize = uploadSize
          storageLimitDialog.open()
        }

        onNotify: showMessage(message)

        onProjectDataChanged: {
          //! if current project has been updated, refresh canvas
          if (projectFullName === projectPanel.activeProjectId) {
            mapCanvas.mapSettings.extentChanged()
          }
        }
    }

    Connections {
        target: __inputProjUtils
        onProjError: {
          showProjError(message)
        }
    }

    Connections {
        target: __inputUtils
        onShowNotificationRequested: {
            showMessage(message)
        }
    }

    FormsStackManager {
        id: formsStackManager

        height: window.height
        width: window.width
        previewHeight: window.height/3

        project: __loader.project

        onEditGeometry: {
          stateManager.state = "edit"
        }

        onClosed: {
          if (stateManager.state !== "edit") {
            updateBrowseDataPanel()

            digitizingHighlight.visible = false
            highlight.visible = false

            if ( browseDataPanel.visible ) browseDataPanel.focus = true
            else mainPanel.focus = true
          }
        }
    }

    Connections {
        target: __loader
        onProjectWillBeReloaded: {
            formsStackManager.reload()
        }
    }

    LoadingIndicator {
        id: loadingIndicator
        visible: false
        width: parent.width
        height: 7 * QgsQuick.Utils.dp
        z: zPanel + 1000 // the most top
    }

    Connections {
        target: __loader
        onLoadingStarted: projectLoadingScreen.visible = true
        onLoadingFinished: projectLoadingScreen.visible = false
        onProjectReloaded: {
          // clear all previous references to old project (if we don't clear references to the previous project,
          // highlights may end up with dangling pointers to map layers and cause crashes)
          highlight.featureLayerPair = null
          digitizingHighlight.featureLayerPair = null
        }
    }

    ProjectLoadingScreen {
      id: projectLoadingScreen
      anchors.fill: parent
      visible: false
      z: zPanel + 1000 // the most top
    }

}
