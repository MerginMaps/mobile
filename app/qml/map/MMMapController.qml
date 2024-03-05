/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Shapes

import mm 1.0 as MM

import "../components"
import "./components"
import "../dialogs"

Item {
  id: root

  // mapExtentOffset represents a height (or a portion) of canvas which is occupied by some other component
  // like preview panel or stakeout panel. Map extent thus must be calculated regarding to this
  // offset in order to not highlight features in the occupied area, but rather move canvas
  property real mapExtentOffset

  readonly property alias gpsIndicatorColor: gpsStateGroup.indicatorColor

  readonly property alias mapSettings: mapCanvas.mapSettings
  readonly property alias compass: deviceCompass

  property bool isTrackingPosition: trackingManager?.isTrackingPosition ?? false
  property bool isStreaming: recordingToolsLoader.active ? recordingToolsLoader.item.recordingMapTool.recordingType = RecordingMapTool.StreamMode : false

  property MM.PositionTrackingManager trackingManager: tracking.item?.manager ?? null

  signal featureIdentified( var pair )
  signal featuresIdentified( var pairs )
  signal nothingIdentified()

  signal recordingStarted()
  signal recordingFinished( var pair )
  signal recordingCanceled()

  signal editingGeometryStarted()
  signal editingGeometryFinished( var pair )
  signal editingGeometryCanceled()

  signal recordInLayerFeatureStarted()
  signal recordInLayerFeatureFinished( var pair )
  signal recordInLayerFeatureCanceled()

  signal splittingStarted()
  signal splittingFinished()
  signal splittingCanceled()

  signal notify( string message )

  signal stakeoutStarted( var pair )
  signal accuracyButtonClicked()

  signal localChangesPanelRequested()

  signal openTrackingPanel()
  signal openStreamingPanel()

  states: [
    State {
      name: "view"
    },
    State {
      name: "record"
    },
    State {
      // recording feature in specific layer without option to change the recording layer,
      // used to create linked features in relations and value relations
      name: "recordInLayer"
    },
    State {
      name: "edit" // of existing feature
    },
    State {
      name: "split" // of existing feature
    },
    State {
      name: "stakeout"
    },
    State {
      name: "inactive" // ignores touch input
    }
  ]

  onStateChanged: {
    switch ( state ) {

    case "record": {
      root.recordingStarted()
      break
    }

    case "recordInLayer": {
      root.recordInLayerFeatureStarted()
      root.hideHighlight()
      break
    }

    case "edit": {
      root.editingGeometryStarted()
      root.hideHighlight()
      break
    }

    case "split": {
      howtoSplittingBanner.show()
      root.splittingStarted()
      break
    }

    case "view": {
      root.hideHighlight()
      break
    }

    case "stakeout": {
      root.hideHighlight()
      root.stakeoutStarted( internal.stakeoutTarget )
      break
    }

    case "inactive": {
      break
    }
    }
  }

  state: "view"

  ColumnLayout {
    id: mapLayout

    anchors.fill: parent

    spacing: 0

    Item {
      id: canvasRoot

      Layout.fillWidth: true
      Layout.fillHeight: true

      Rectangle {
        id: canvasBackground
        color: __style.whiteColor
        anchors.fill: parent
      }

      MMMapCanvas {
        id: mapCanvas

        anchors.fill: canvasRoot

        mapSettings.project: __activeProject.qgsProject

        MM.IdentifyKit {
          id: identifyKit

          mapSettings: mapCanvas.mapSettings
          identifyMode: MM.IdentifyKit.TopDownAll
        }

        onClicked: function( point ) {
          if ( root.state === "view" )
          {
            let screenPoint = Qt.point( point.x, point.y )
            let pair = identifyKit.identifyOne( screenPoint )

            if ( pair.valid )
            {
              root.select( pair )
              root.featureIdentified( pair )
            }
            else
            {
              root.hideHighlight()
              root.nothingIdentified()
            }
          }
        }

        onLongPressed: function( point ) {
          // Alter position of simulated provider
          if ( __positionKit.positionProvider && __positionKit.positionProvider.id() === "simulated" )
          {
            __positionKit.positionProvider.setPosition( __inputUtils.mapPointToGps( Qt.point( point.x, point.y ), mapCanvas.mapSettings ) )
          }

          if ( root.state === "view" )
          {
            let screenPoint = Qt.point( point.x, point.y )
            let pairs = identifyKit.identify( screenPoint )

            if ( !pairs.isEmpty )
            {
              root.featuresIdentified( pairs )
            }
            else
            {
              root.hideHighlight()
              root.nothingIdentified()
            }
          }
        }
      }

      MM.Compass { id: deviceCompass }

      StateGroup {
        id: gpsStateGroup

        property color indicatorColor: __style.negativeColor

        states: [
          State {
            name: "good" // GPS provides position AND horizontal accuracy is below set tolerance (threshold)
            when: __positionKit.hasPosition && __positionKit.horizontalAccuracy > 0 && __positionKit.horizontalAccuracy <= __appSettings.gpsAccuracyTolerance
            PropertyChanges {
              target: gpsStateGroup
              indicatorColor: __style.positiveColor
            }
          },
          State {
            name: "low" // below accuracy tolerance OR GPS does not provide horizontal accuracy
            when: __positionKit.hasPosition &&  (__positionKit.horizontalAccuracy < 0 || __positionKit.horizontalAccuracy > __appSettings.gpsAccuracyTolerance )
            PropertyChanges {
              target: gpsStateGroup
              indicatorColor: __style.warningColor
            }
          },
          State {
            name: "unavailable" // GPS does not provide position
            when: !__positionKit.hasPosition
            PropertyChanges {
              target: gpsStateGroup
              indicatorColor: __style.negativeColor
            }
          }
        ]
      }

      // TOP elements
      MMMapLoadingIndicator {
        id: loadingIndicator

        width: mapCanvas.width
        anchors.top: canvasRoot.top
        running: mapCanvas.isRendering && root.state !== "inactive"
      }

      MMMapButton {
        id: backButton

        height: visible ? __style.mapItemHeight : 0
        anchors {
          top: loadingIndicator.bottom
          topMargin: __style.margin8
          left: parent.left
          leftMargin: __style.mapButtonsMargin
        }
        visible: internal.isInRecordState || root.state === "split"
        iconSource: __style.backIcon

        onClicked: {
          if ( root.state === "edit" || root.state === "record" || root.state === "recordInLayer" ) {
            if ( recordingToolsLoader.item.hasChanges() ) {
              cancelEditDialog.open()
            }
            else {
              recordingToolsLoader.item.discardChanges()
            }
          }
          else if ( root.state === "split" ) {
            howtoSplittingBanner.hide()
            root.splittingCanceled()
            root.state = "view"
          }
        }
      }

      MMMapPicker {
        id: mapPicker

        width: Math.min( parent.width - backButton.width - ( 3 * __style.mapButtonsMargin ), 500 * __dp )
        height: visible ? __style.mapItemHeight : 0
        anchors {
          top: loadingIndicator.bottom
          topMargin: __style.margin8
          left: backButton.right
          leftMargin: __style.mapButtonsMargin
        }
        visible: root.state === "record"
        text: __activeLayer.layerName
        leftIconSource: __inputUtils.loadIconFromLayer( __activeLayer.layer )

        onClicked: activeLayerPanel.open()
      }

      MMMapBlurLabel {
        id: howtoSplittingBanner

        visible: false
        width: parent.width - 2 * __style.pageMargins
        height: visible ?  __style.row40 : 0
        anchors.top: backButton.bottom
        anchors.topMargin: __style.margin8
        sourceItem: mapCanvas
        text: qsTr( "Create line to split the selected feature" )
      }

      MMMapBlurLabel {
        id: howtoEditingBanner

        width: parent.width - 2 * __style.pageMargins
        height: visible ?  __style.row40 : 0
        anchors.top: howtoSplittingBanner.bottom
        anchors.topMargin: __style.margin8
        sourceItem: mapCanvas
        text: qsTr( "Select some point to start editing the geometry" )
      }

      MMMapBlurLabel {
        id: redrawGeometryBanner

        width: parent.width - 2 * __style.pageMargins
        height: visible ?  __style.row40 : 0
        anchors.top: howtoEditingBanner.bottom
        anchors.topMargin: __style.margin8
        sourceItem: mapCanvas
        text: qsTr( "Record new geometry for the feature" )
      }

      MMMapScaleBar {
        id: scaleBar

        mapSettings: mapCanvas.mapSettings
        sourceItem: mapCanvas
        preferredWidth: Math.min( window.width, 180 * __dp )

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: redrawGeometryBanner.bottom
        anchors.topMargin: __style.margin8
      }
      // END OF: TOP elements

      MMHighlight {
        id: identifyHighlight

        visible: root.state === "view"
        anchors.fill: mapCanvas

        mapSettings: mapCanvas.mapSettings
      }

      Loader {
        id: tracking

        anchors.fill: mapCanvas
        asynchronous: true
        active: false

        sourceComponent: Component {
          Item {
            property alias manager: trackingManager

            MM.PositionTrackingManager {
              id: trackingManager

              variablesManager: __variablesManager
              qgsProject: __activeProject.qgsProject

              onTrackingErrorOccured: (message) => {
                                        notify( message )
                                      }

              onAbort: () => {
                         root.setTracking( false )
                       }
            }

            MM.PositionTrackingHighlight {
              id: trackingHighlight

              mapPosition: mapPositionSource.mapPosition
              trackedGeometry: __inputUtils.transformGeometryToMapWithCRS( trackingManager.trackedGeometry, trackingManager.crs(), mapCanvas.mapSettings )
            }

            MMHighlight {
              height: mapCanvas.height
              width: mapCanvas.width

              markerColor: __style.sunsetColor
              lineColor: __style.sunsetColor
              lineWidth: MMHighlight.LineWidths.Narrow

              mapSettings: mapCanvas.mapSettings
              geometry: trackingHighlight.highlightGeometry
            }

            Component.onCompleted: {
              trackingManager.trackingBackend = trackingManager.constructTrackingBackend( __activeProject.qgsProject, __positionKit )
            }

            Connections {
              target: __activeProject

              function onProjectWillBeReloaded() {
                // simply stop tracking
                root.setTracking( false )
              }
            }
          }
        }
      }

      Loader {
        id: stakeoutLoader

        anchors.fill: mapCanvas

        asynchronous: true
        active: root.state === "stakeout"

        sourceComponent: stakeoutToolsComponent
      }

      MM.MapPosition {
        id: mapPositionSource

        mapSettings: mapCanvas.mapSettings
        positionKit: __positionKit
        onScreenPositionChanged: root.updatePosition()
      }

      MM.PositionDirection {
        id: positionDirectionSource

        positionKit: __positionKit
        compass: deviceCompass
      }

      MMPositionMarker {
        id: positionMarker

        xPos: mapPositionSource.screenPosition.x
        yPos: mapPositionSource.screenPosition.y
        hasDirection: positionDirectionSource.hasDirection

        direction: positionDirectionSource.direction
        hasPosition: __positionKit.hasPosition

        horizontalAccuracy: __positionKit.horizontalAccuracy
        accuracyRingSize: mapPositionSource.screenAccuracy

        trackingMode: root.state !== "inactive" && tracking.active
      }

      Loader {
        id: recordingToolsLoader

        anchors.fill: mapCanvas

        asynchronous: true
        active: internal.isInRecordState

        sourceComponent: recordingToolsComponent
      }

      Loader {
        id: splittingLoader

        anchors.fill: mapCanvas

        asynchronous: true
        active: root.state === "split"

        sourceComponent: splittingToolsComponent
      }

      MMMapButton {
        id: gpsButton

        anchors {
          right: parent.right
          rightMargin: __style.mapButtonsMargin
          bottom: parent.bottom
          bottomMargin: internal.bottomMapButtonsMargin
        }

        visible: root.mapExtentOffset > 0 ? false : true

        iconSource: __style.gpsIcon

        onClicked: {
          if ( gpsStateGroup.state === "unavailable" ) {
            __notificationModel.addError( qsTr( "GPS currently unavailable" ) )
            return
          }

          if ( recordingToolsLoader.active ) {
            recordingToolsLoader.item.recordingMapTool.centeredToGPS = true
          }

          mapSettings.setCenter( mapPositionSource.mapPosition )
        }
      }

      MMMapButton {
        id: moreToolsButton

        anchors {
          right: parent.right
          rightMargin: __style.mapButtonsMargin
          bottom: gpsButton.visible ? gpsButton.top : parent.bottom
          bottomMargin: internal.bottomMapButtonsMargin
        }

        visible: internal.isInRecordState && internal.isSpatialLayer && !root.isStreaming

        iconSource: __style.moreIcon

        onClicked: moreToolsMenu.open()
      }

      MMMenuDrawer {
        id: moreToolsMenu

        title: qsTr("More options")
        model: ObjectModel {
          MMToolbarMenuButton {
            height: visible ? __style.menuDrawerHeight/2 : 0
            width: window.width

            text: qsTr("Split geometry")
            iconSource: __style.splitGeometryIcon
            visible: !internal.isPointLayer && !root.isStreaming

            onClicked: root.toggleSplitting()
          }

          MMToolbarMenuButton {
            height: __style.menuDrawerHeight/2
            width: window.width

            visible: !root.isStreaming
            text: qsTr("Redraw geometry")
            iconSource: __style.redrawGeometryIcon

            onClicked: root.toggleRedraw()
          }

          MMToolbarMenuButton {
            height: visible ? __style.menuDrawerHeight/2 : 0
            width: window.width

            text: qsTr("Streaming mode")
            iconSource: __style.streamingIcon
            visible: !internal.isPointLayer
            rightText: root.isStreaming ? qsTr("active") : ""

            onClicked: root.openStreamingPanel()
          }
        }
        onClicked: moreToolsMenu.close()
      }

      MessageDialog {
        id: cancelEditDialog

        title: qsTr( "Discard the changes?" )
        text: {
          if ( root.state === "edit" ) {
            return qsTr( "Clicking ‘Yes’ discards your changes to the geometry. If you would like " +
                        "to save the changes instead, hit ‘No’ and then ‘Done’ in the toolbar." )
          }
          else if ( root.state === "record" || root.state === "recordInLayer" ) {
            return qsTr( "Clicking ‘Yes’ discards your new geometry and no feature will be saved. " +
                        "If you would like to save the geometry instead, hit ‘No’ and then ‘Done’ " +
                        "in the toolbar." )
          }
          return ""
        }

        buttons: MessageDialog.Yes | MessageDialog.No

        onButtonClicked: function(clickedButton) {
          if ( clickedButton === MessageDialog.Yes ) {
            recordingToolsLoader.item.discardChanges()
          }
          cancelEditDialog.close()
        }
      }

      MMSplittingFailedDialog {
        id: splittingFailedDialog
      }

      MMMapLabel {
        id: streamingModeButton

        anchors {
          left: parent.left
          leftMargin: __style.mapButtonsMargin
          bottom: positionTrackingButton.visible ? positionTrackingButton.top : accuracyButton.top
          bottomMargin: internal.bottomMapButtonsMargin
        }

        visible: root.state !== "inactive" && root.isStreaming
        iconSource: __style.streamingIcon

        text: qsTr("streaming")
        textBgColorInverted: true

        onClicked: function( mouse ) {
          mouse.accepted = true
          root.openSteamingPanel()
        }
      }

      MMMapLabel {
        id: positionTrackingButton

        anchors {
          left: parent.left
          leftMargin: __style.mapButtonsMargin
          bottom: accuracyButton.top
          bottomMargin: internal.bottomMapButtonsMargin
        }

        visible: root.state !== "inactive" && tracking.active
        iconSource: __style.positionTrackingIcon

        text: {
          if (visible) {
            // TODO make some merge with main.qml:trackingPrivate.getStartingTime()
            let date = root.trackingManager?.startTime
            if ( date ) {
              return date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds()
            }
          }
          else
            return ""
        }
        textBgColorInverted: true

        onClicked: function( mouse ) {
          mouse.accepted = true
          root.openTrackingPanel()
        }
      }

      MMMapLabel {
        id: accuracyButton
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: 20
        anchors.bottomMargin: internal.bottomMapButtonsMargin

        onClicked: root.accuracyButtonClicked()

        iconSource: __style.satelliteIcon

        bgColor: {
          if ( gpsStateGroup.state === "good" ) {
            return __style.positiveColor
          }
          else if ( gpsStateGroup.state === "low" ) {
            return __style.warningColor
          }
          return __style.negativeColor
        }

        textColor: {
          if ( gpsStateGroup.state === "good" ) {
            return __style.forestColor
          }
          else if ( gpsStateGroup.state === "low" ) {
            return __style.earthColor
          }
          return __style.grapeColor
        }

        visible: {
          if ( root.mapExtentOffset > 0 ) return false

          if ( __positionKit.positionProvider && __positionKit.positionProvider.type() === "external" ) {
            // for external receivers we want to show gps panel and accuracy button
            // even when the GPS receiver is not sending position data
            return true
          }
          else {
            if ( gpsStateGroup.state !== "unavailable" ) {
              return true
            }
            else {
              return false
            }
          }
        }

        text: {
          if ( !__positionKit.positionProvider )
          {
            return ""
          }
          else if ( __positionKit.positionProvider.type() === "external" )
          {
            if ( __positionKit.positionProvider.state === PositionProvider.Connecting )
            {
              return qsTr( "Connecting to %1" ).arg( __positionKit.positionProvider.name() )
            }
            else if ( __positionKit.positionProvider.state === PositionProvider.WaitingToReconnect )
            {
              return __positionKit.positionProvider.stateMessage
            }
            else if ( __positionKit.positionProvider.state === PositionProvider.NoConnection )
            {
              return __positionKit.positionProvider.stateMessage
            }
          }

          if ( !__positionKit.hasPosition )
          {
            return qsTr( "Connected, no position" )
          }
          else if ( Number.isNaN( __positionKit.horizontalAccuracy ) || __positionKit.horizontalAccuracy < 0 )
          {
            return qsTr( "Unknown accuracy" )
          }

          let accuracyText = __inputUtils.formatNumber( __positionKit.horizontalAccuracy, __positionKit.horizontalAccuracy > 1 ? 1 : 2 ) + " m"
          if ( __appSettings.gpsAntennaHeight > 0 )
          {
            let gpsText = Number( __appSettings.gpsAntennaHeight.toFixed( 3 ) ) + " m"
            return gpsText + " / " + accuracyText
          }
          else
          {
            return accuracyText
          }
        }
      }

      MMListDrawer {
        id: activeLayerPanel

        title: qsTr( "Choose Active Layer" )

        model: __recordingLayersModel
        activeValue: __activeLayer.layerId

        valueRole: "layerId"
        textRole: "layerName"

        // TODO select active layer in list?

        noItemsDelegate: MMMessage {
          image: __style.negativeMMSymbolImage
          description: qsTr( "Could not find any editable layers in the project." )
          linkText: qsTr( "See how to enable digitizing in your project." )
          link: __inputHelp.howToEnableDigitizingLink
        }

        onClicked: function ( layerId ) {
          __activeProject.setActiveLayer( __recordingLayersModel.layerFromLayerId( layerId ) )
          activeLayerPanel.close()
        }
      }

      Connections {
        target: mapCanvas.mapSettings
        function onExtentChanged() {
          scaleBar.visible = true
        }
      }

      Component {
        id: recordingToolsComponent

        MMRecordingTools {
          anchors.fill: parent

          map: mapCanvas
          positionMarkerComponent: positionMarker

          activeFeature: root.state === "edit" ? internal.featurePairToEdit.feature : __inputUtils.emptyFeature()

          onCanceled: {
            howtoEditingBanner.hide()

            if ( root.state === "record" )
            {
              root.recordingCanceled()
            }
            else if ( root.state === "edit" )
            {
              root.editingGeometryCanceled()
            }
            else if ( root.state === "recordInLayer" )
            {
              root.recordInLayerFeatureCanceled()
            }

            root.state = "view"
          }

          onDone: function( featureLayerPair ) {
            howtoEditingBanner.hide()

            if ( root.state === "record" )
            {
              root.recordingFinished( featureLayerPair )
            }
            else if ( root.state === "edit" )
            {
              root.editingGeometryFinished( featureLayerPair )
            }
            else if ( root.state === "recordInLayer" )
            {
              root.recordInLayerFeatureFinished( featureLayerPair )
            }

            root.state = "view"
          }

          Component.onCompleted: {
            if ( root.state !== "edit" )
            {
              // center to GPS
              if ( gpsStateGroup.state === "unavailable" ) {
                __notificationModel.addError( qsTr( "GPS currently unavailable." ) )
                return
              }

              recordingMapTool.centeredToGPS = true
              mapSettings.setCenter( mapPositionSource.mapPosition )
            }
          }
        }
      }

      Component {
        id: stakeoutToolsComponent

        MMStakeoutTools {
          anchors.fill: parent

          map: mapCanvas
          mapExtentOffset: root.mapExtentOffset

          target: internal.stakeoutTarget
        }
      }

      Component {
        id: splittingToolsComponent

        MMSplittingTools {
          anchors.fill: parent

          map: mapCanvas
          featureToSplit: internal.featurePairToEdit

          onDone: function (success) {
            // close all feature forms, show banner if it went fine or not
            howtoSplittingBanner.hide()

            if ( success )
            {
              __notificationModel.addSuccess( qsTr( "Splitting done successfully" ) )
            }
            else
            {
              splittingFailedDialog.open()
            }


            root.splittingFinished()

            root.state = "view"
          }

          onCanceled: {
            // go back to feature form
            howtoSplittingBanner.hide()

            root.splittingCanceled()

            root.state = "view"
          }
        }
      }

      Connections {
        target: __activeProject

        function onStartPositionTracking() {

          if ( !tracking.active )
          {
            root.setTracking( true )
          }
        }
      }

      QtObject {
        id: internal
        // private properties - not accessible by other components

        property var featurePairToEdit // we are editing geometry of this feature layer pair
        property bool isSpatialLayer: internal.featurePairToEdit ? __inputUtils.isSpatialLayer( internal.featurePairToEdit.layer ) : false // featurePairToEdit is valid and contains layer with features with geometry
        property bool isPointLayer: internal.featurePairToEdit ? __inputUtils.isPointLayer( internal.featurePairToEdit.layer ) : false // featurePairToEdit is valid and contains layer with point geometry features

        property var extentBeforeStakeout // extent that we return to once stakeout finishes
        property var stakeoutTarget

        property bool isInRecordState: root.state === "record" || root.state === "recordInLayer" || root.state === "edit"

        // bottomMapButtonsMargin represents distance between toolbar and the bottom of the map buttons (gps and accuracy)
        // in case the toolbar has the overlaying "record" button, we need to move these two buttons a little higher
        property real bottomMapButtonsMargin: {
          let mapDrawersOffset = root.mapExtentOffset
          let mapMinMargin = __style.mapButtonsMargin
          let toolbarRequiredOffset = 0

          if ( root.state !== "view" && __inputUtils.isLineLayer( __activeLayer.vectorLayer ) ) {
            toolbarRequiredOffset = 16 * __dp
          }

          return mapDrawersOffset + mapMinMargin + toolbarRequiredOffset
        }
      }
    }
  }

  function select( featurepair ) {
    root.centerToPair( featurepair, true )
    root.highlightPair( featurepair )
  }

  function record() {
    state = "record"
  }

  function recordInLayer( layer, parentpair ) {
    __activeProject.setActiveLayer( layer )
    root.centerToPair( parentpair )
    state = "recordInLayer"
  }

  function edit( featurepair ) {
    __activeProject.setActiveLayer( featurepair.layer )
    root.centerToPair( featurepair )
    howtoEditingBanner.show()

    internal.featurePairToEdit = featurepair
    state = "edit"
  }


  function toggleRedraw() {
    redraw(internal.featurePairToEdit)
  }

  function redraw( featurepair ) {
    __activeProject.setActiveLayer( featurepair.layer )
    root.centerToPair( featurepair )
    redrawGeometryBanner.show()

    // clear feature geometry
    internal.featurePairToEdit = __inputUtils.changeFeaturePairGeometry( featurepair, __inputUtils.emptyGeometry() )

    state = "edit"
  }

  function toggleSplitting() {
    split(internal.featurePairToEdit)
  }

  function split( featurepair ) {
    root.centerToPair( featurepair )

    internal.featurePairToEdit = featurepair
    state = "split"
  }

  function stakeout( featurepair )
  {
    internal.extentBeforeStakeout = mapCanvas.mapSettings.extent
    internal.stakeoutTarget = featurepair
    state = "stakeout"
  }

  function toggleStreaming() {
    // start/stop the streaming mode
    if ( recordingToolsLoader.active ) {
      recordingToolsLoader.item.toggleStreaming()
    }
  }

  function autoFollowStakeoutPath()
  {
    if ( state === "stakeout" )
    {
      stakeoutLoader.item.autoFollow()
    }
  }

  function stopStakeout()
  {
    state = "view"

    // go back to state before starting stakeout
    root.highlightPair( internal.stakeoutTarget )
    mapCanvas.mapSettings.extent = internal.extentBeforeStakeout
  }

  function centerToPair( pair, considerMapExtentOffset = false ) {
    if ( considerMapExtentOffset )
      var mapExtentOffsetRatio = mapExtentOffset / mapCanvas.height
    else
      mapExtentOffsetRatio = 0

    __inputUtils.setExtentToFeature( pair, mapCanvas.mapSettings, mapExtentOffsetRatio )
  }

  function highlightPair( pair ) {
    let geometry = __inputUtils.extractGeometry( pair )
    identifyHighlight.geometry = __inputUtils.transformGeometryToMapWithLayer( geometry, pair.layer, mapCanvas.mapSettings )
  }

  function hideHighlight() {
    identifyHighlight.geometry = null
  }

  function centerToPosition() {
    if ( __positionKit.hasPosition ) {
      mapCanvas.mapSettings.setCenter( mapPositionSource.mapPosition )
    }
    else {
      __notificationModel.addWarning( qsTr( "GPS currently unavailable." ) )
    }
  }

  function isPositionOutOfExtent() {
    let minDistanceToScreenEdge = 64 * __dp
    return ( ( mapPositionSource.screenPosition.x < minDistanceToScreenEdge ) ||
            ( mapPositionSource.screenPosition.y < minDistanceToScreenEdge ) ||
            ( mapPositionSource.screenPosition.x > mapCanvas.width - minDistanceToScreenEdge ) ||
            ( mapPositionSource.screenPosition.y > mapCanvas.height - minDistanceToScreenEdge )
            )
  }

  function updatePosition() {
    if ( root.state === "view" )
    {
      if ( __appSettings.autoCenterMapChecked && root.isPositionOutOfExtent() )
      {
        root.centerToPosition()
      }
    }
  }

  // Request map repaint
  function refreshMap() {
    mapCanvas.refresh()
  }

  function clear() {
    // clear all previous references to old project (if we don't clear references to the previous project,
    // highlights may end up with dangling pointers to map layers and cause crashes)

    identifyHighlight.geometry = null
  }

  function setTracking( shouldTrack ) {
    if ( shouldTrack ) {
      if ( root.trackingManager ) {
        root.trackingManager.tryAgain()
      }
      else {
        tracking.active = true
      }
    }
    else {
      trackingManager?.commitTrackedPath()
      tracking.active = false
    }
  }
}
