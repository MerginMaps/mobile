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
// bundle if they are only referenced from QgsQuick
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
    title: qsTr("Input")

    property int zMapCanvas: 0
    property int zPanel: 20
    property int zToolkits: 10

    Item {
        id: stateManager
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
                featurePanel.visible = false
                recordToolbar.visible = true
                recordToolbar.extraPanelVisible = false

                __loader.setActiveLayer( featurePanel.feature.layer.name )
                updateRecordToolbar()

                var screenPos = digitizing.pointFeatureMapCoordinates( featurePanel.feature )
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

        featurePanel.show_panel( pair, "Add", "form" )
      }

      stateManager.state = "view"
      digitizing.useGpsPoint = false
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
        var layer = featurePanel.feature.layer
        if (!layer)
        {
            // nothing to do with no active layer
            return
        }

        if (digitizing.hasLineGeometry(layer)) {
            // TODO
        }
        else if (digitizing.hasPointGeometry(layer)) {
            var recordedPoint = getRecordedPoint()
            featurePanel.feature = digitizing.changePointGeometry(featurePanel.feature, recordedPoint, digitizing.useGpsPoint)
            featurePanel.saveFeatureGeom()
            stateManager.state = "view"
            featurePanel.show_panel(featurePanel.feature, "Edit", "form")
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

    function getGpsIndicatorColor() {
        if (positionKit.accuracy <= 0) return InputStyle.softRed
        return positionKit.accuracy < __appSettings.gpsAccuracyTolerance ? InputStyle.softGreen : InputStyle.softOrange
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
      alertDialog.text  = qsTr(message)
      alertDialog.open()
    }

    function updateRecordToolbar()
    {
      if ( !__activeLayer.layer )
        __loader.setActiveLayer( __recordingLayersModel.firstUsableLayer() )

      activeLayerPanel.activeIndex = __recordingLayersModel.indexFromLayer( __activeLayer.layer )
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
          let panelOffsetRatio = featurePanel.previewHeight/window.height
          __inputUtils.setExtentToFeature( feature, mapCanvas.mapSettings, panelOffsetRatio )
      }

      if ( hasGeometry ) {
        highlight.featureLayerPair = feature
        highlight.visible = true
        featurePanel.show_panel( feature, "ReadOnly", "preview" )
      }
      else
        featurePanel.show_panel( feature, "ReadOnly", "form" )
    }

    Component.onCompleted: {
        if (__appSettings.defaultProject) {
            var path = __appSettings.defaultProject ? __appSettings.defaultProject : openProjectPanel.activeProjectPath
            var defaultIndex = __projectsModel.rowAccordingPath(path)
            var isValid = __projectsModel.data(__projectsModel.index(defaultIndex), ProjectModel.IsValid)
            if (isValid && __loader.load(path)) {
                openProjectPanel.activeProjectIndex = defaultIndex !== -1 ? defaultIndex : 0
                __appSettings.activeProject = path
            } else {
                // if default project load failed, delete default setting
                __appSettings.defaultProject = ""
                openProjectPanel.openPanel()
            }
        } else {
            openProjectPanel.openPanel()
        }

        InputStyle.deviceRatio = window.screen.devicePixelRatio
        InputStyle.realWidth = window.width
        InputStyle.realHeight = window.height

        __loader.positionKit = positionKit
        __loader.recording = digitizing.recording
        __loader.mapSettings = mapCanvas.mapSettings

        console.log("Completed Running!")
    }

    QgsQuick.MapCanvas {
      id: mapCanvas

      height: parent.height - mainPanel.height
      width: parent.width
      z: zMapCanvas

      mapSettings.project: __loader.project

      QgsQuick.IdentifyKit {
        id: identifyKit
        mapSettings: mapCanvas.mapSettings
        identifyMode: QgsQuick.IdentifyKit.TopDownAll
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
          selectFeature(res, ( mouse.y > window.height - featurePanel.previewHeight ) )
        } else if (featurePanel.visible) { // closes feature/preview panel when there is nothing to show
          featurePanel.visible = false
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

      property bool hasPolygon: featureLayerPair !== null ? digitizing.hasPolygonGeometry(featureLayerPair.layer) : false

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
    QgsQuick.PositionKit {
      id: positionKit
      mapSettings: mapCanvas.mapSettings
      simulatePositionLongLatRad: __use_simulated_position ? [-2.9207148, 51.3624998, 0.05] : []

      onScreenPositionChanged: {
        if (digitizing.useGpsPoint || (__appSettings.autoCenterMapChecked && isPositionOutOfExtent(mainPanel.height))) {
            var useGpsPoint = digitizing.useGpsPoint
            mapCanvas.mapSettings.setCenter(positionKit.projectedPosition);
            // sets previous useGpsPoint value, because setCenter triggers extentChanged signal which changes this property
            digitizing.useGpsPoint = useGpsPoint
        }
      }
    }

    PositionMarker {
      id: positionMarker
      positionKit: positionKit
      z: zMapCanvas + 2
    }

    DigitizingController {
        id: digitizing
        positionKit: positionMarker.positionKit
        layer: recordToolbar.activeVectorLayer
        lineRecordingInterval: __appSettings.lineRecordingInterval
        mapSettings: mapCanvas.mapSettings

        onRecordingChanged: {
            __loader.recording = digitizing.recording
        }
    }

    // Highlighting a new feature while digitizing
    Connections {
        target: digitizing.recordingFeatureModel
        onFeatureLayerPairChanged: {
            if (digitizing.recording) {
                digitizingHighlight.visible = true
                digitizingHighlight.featureLayerPair = digitizing.recordingFeatureModel.featureLayerPair
            }
        }
    }

    MainPanel {
        id: mainPanel
        width: window.width
        height: InputStyle.rowHeightHeader
        z: zToolkits + 1
        y: window.height - height

        gpsIndicatorColor: getGpsIndicatorColor()

        onOpenProjectClicked: openProjectPanel.openPanel()
        onOpenMapThemesClicked: mapThemesPanel.visible = true
        onMyLocationClicked: {
          mapCanvas.mapSettings.setCenter(positionKit.projectedPosition)
          digitizing.useGpsPoint = true
        }
        onMyLocationHold: {
            __appSettings.autoCenterMapChecked =!__appSettings.autoCenterMapChecked
            popup.text = __appSettings.autoCenterMapChecked ?  qsTr("Autocenter mode on") : qsTr("Autocenter mode off")
            popup.open()
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
                popup.text = qsTr("No editable layers!")
                popup.open()
            }
        }
    }

    Connections {
      target: __activeLayer
      onActiveLayerChanged: {
        updateRecordToolbar()
      }
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
        onVisibleChanged: if (visible) digitizing.manualRecording = true

        onAddClicked: {
            if (stateManager.state === "record") {
                recordFeature()
            } else if (stateManager.state === "edit") {
                editFeature()
            }
        }

        onGpsSwitchClicked: {
            if (!positionKit.hasPosition) {
                popup.text = qsTr("The GPS is currently not available")
                popup.open()
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
            }
        }

        onCancelClicked: {
            if (stateManager.state === "edit") {
                featurePanel.show_panel(featurePanel.feature, "Edit", "form")
            }
            stateManager.state = "view"
            digitizing.stopRecording();
            digitizingHighlight.visible = false
        }

        onRemovePointClicked: {
            digitizing.removeLastPoint()
        }

         onStopRecordingClicked: {
             digitizing.stopRecording()
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

    Rectangle {
        id: gpsAccuracy
        width: scaleBar.textWidth
        height: 35 * QgsQuick.Utils.dp
        x: 0
        y: window.height - 2 * (35 * QgsQuick.Utils.dp)
        z: zToolkits
        color: InputStyle.clrPanelBackground
        opacity: InputStyle.panelOpacity
        visible: false
        Text  {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            color: scaleBar.barColor
            font.pixelSize: scaleBar.height - 2 * scaleBar.lineWidth
            text: QgsQuick.Utils.formatDistance( positionKit.accuracy, positionKit.accuracyUnits, 0 )
        }
    }

    MerginProjectPanel {
        id: openProjectPanel
        height: window.height
        width: window.width
        z: zPanel

        onVisibleChanged: {
          if (openProjectPanel.visible)
            openProjectPanel.focus = true
          else
          {
            mainPanel.focus = true
          }
        }

        onActiveProjectIndexChanged: {
            openProjectPanel.activeProjectPath = __projectsModel.data(__projectsModel.index(openProjectPanel.activeProjectIndex), ProjectModel.Path)
            __appSettings.defaultProject = openProjectPanel.activeProjectPath
            __appSettings.activeProject = openProjectPanel.activeProjectPath
            __loader.load(openProjectPanel.activeProjectPath)
        }
    }

    ActiveLayerPanel {
        id: activeLayerPanel
        height: window.height/2
        width: window.width
        edge: Qt.BottomEdge
        z: zPanel

        onActiveLayerChangeRequested: {
          __loader.setActiveLayer( __recordingLayersModel.layerFromIndex( index ) )
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

    MessageDialog {
        id: alertDialog
        title: qsTr("Synchronization error")
        onAccepted: alertDialog.close()
    }

    Connections {
        target: __merginApi
        onNetworkErrorOccurred: {
            var msg = message ? message : qsTr("Unknown network error")
            showAsDialog ? showDialog(msg) : showMessage(msg)
        }
        onNotify: {
            showMessage(message)
        }

        onProjectDataChanged: {
          var projectName = __projectsModel.data(__projectsModel.index(openProjectPanel.activeProjectIndex), ProjectModel.ProjectName)
          var projectNamespace = __projectsModel.data(__projectsModel.index(openProjectPanel.activeProjectIndex), ProjectModel.ProjectNamespace)
          var currentProjectFullName = __merginApi.getFullProjectName(projectNamespace, projectName)

          //! if current project has been updated, refresh canvas
          if (projectFullName === currentProjectFullName) {
            mapCanvas.mapSettings.extentChanged()
          }
        }
    }

    Connections {
        target: __inputUtils
        onShowNotificationRequested: {
            showMessage(message)
        }
    }

    FeaturePanel {
        id: featurePanel
        height: window.height
        width: window.width
        mapSettings: mapCanvas.mapSettings
        panelHeight: window.height
        previewHeight: window.height/3
        project: __loader.project
        z: 0 // to featureform editors be visible

        onVisibleChanged: {
            if ( !visible ) {
                digitizingHighlight.visible = false
                highlight.visible = false

              if (stateManager.state !== "edit") {
                if ( browseDataPanel.visible ) browseDataPanel.focus = true
                else mainPanel.focus = true
              }
            }
            else featurePanel.forceActiveFocus()
        }

        onEditGeometryClicked: {
            stateManager.state = "edit"
        }

        onPanelClosed: {
          updateBrowseDataPanel()
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
    }

    ProjectLoadingScreen {
      id: projectLoadingScreen
      anchors.fill: parent
      visible: false
      z: zPanel + 1000 // the most top
    }

}
