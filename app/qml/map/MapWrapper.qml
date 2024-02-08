/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import lc 1.0
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Shapes

import ".."
import "../components"
import "../dialogs"
import "../banners"

import notificationType 1.0

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
  property PositionTrackingManager trackingManager: tracking.item?.manager ?? null

  signal featureIdentified( var pair )
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

    Rectangle {
      id: trackingIndicator

      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.rowHeightMedium

      z: 10 // in order to write this indicator on top of all map components

      visible: root.state !== "inactive" && tracking.active

      color: InputStyle.contrastFontColor

      Text {
        anchors.fill: parent

        text: qsTr( "Position tracking is running" )

        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter

        color: InputStyle.fontColor

        font.pixelSize: InputStyle.fontPixelSizeNormal
      }

      MouseArea {
        anchors.fill: parent
        onClicked: function( mouse ) {
          mouse.accepted = true
          root.openTrackingPanel()
        }
      }
    }

    Item {
      id: canvasRoot

      Layout.fillWidth: true
      Layout.fillHeight: true

      Rectangle {
        // background
        color: InputStyle.clrPanelMain
        anchors.fill: parent
      }

      MMMapCanvas {
        id: mapCanvas

        anchors.fill: canvasRoot

        mapSettings.project: __activeProject.qgsProject

        IdentifyKit {
          id: identifyKit

          mapSettings: mapCanvas.mapSettings
          identifyMode: IdentifyKit.TopDownAll
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
        }
      }

      Compass { id: deviceCompass }

      StateGroup {
        id: gpsStateGroup

        property color indicatorColor: InputStyle.softRed

        states: [
          State {
            name: "good" // GPS provides position AND horizontal accuracy is below set tolerance (threshold)
            when: __positionKit.hasPosition && __positionKit.horizontalAccuracy > 0 && __positionKit.horizontalAccuracy <= __appSettings.gpsAccuracyTolerance
            PropertyChanges {
              target: gpsStateGroup
              indicatorColor: InputStyle.softGreen
            }
          },
          State {
            name: "low" // below accuracy tolerance OR GPS does not provide horizontal accuracy
            when: __positionKit.hasPosition &&  (__positionKit.horizontalAccuracy < 0 || __positionKit.horizontalAccuracy > __appSettings.gpsAccuracyTolerance )
            PropertyChanges {
              target: gpsStateGroup
              indicatorColor: InputStyle.softOrange
            }
          },
          State {
            name: "unavailable" // GPS does not provide position
            when: !__positionKit.hasPosition
            PropertyChanges {
              target: gpsStateGroup
              indicatorColor: InputStyle.softRed
            }
          }
        ]
      }

      LoadingIndicator {
        width: mapCanvas.width
        height: InputStyle.mapLoadingIndicatorHeight

        anchors.top: canvasRoot.top

        visible: mapCanvas.isRendering && root.state !== "inactive"
      }

      MMMapScaleBar {
        id: scaleBar

        mapSettings: mapCanvas.mapSettings
        sourceItem: mapCanvas

        // height: InputStyle.scaleBarHeight
        preferredWidth: Math.min( window.width, 180 * __dp )

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: canvasRoot.top
        anchors.topMargin: InputStyle.smallGap
      }

      Highlight {
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

            PositionTrackingManager {
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

            PositionTrackingHighlight {
              id: trackingHighlight

              mapPosition: mapPositionSource.mapPosition
              trackedGeometry: __inputUtils.transformGeometryToMapWithCRS( trackingManager.trackedGeometry, trackingManager.crs(), mapCanvas.mapSettings )
            }

            Highlight {
              height: mapCanvas.height
              width: mapCanvas.width

              markerColor: __style.sunsetColor
              lineColor: __style.sunsetColor
              lineWidth: Highlight.LineWidths.Narrow

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

      MapPosition {
        id: mapPositionSource

        mapSettings: mapCanvas.mapSettings
        positionKit: __positionKit
        onScreenPositionChanged: root.updatePosition()
      }

      PositionDirection {
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

      AutoHideBanner { // TODO: Replace by MapBlurItem later
        id: howtoSplittingBanner

        width: parent.width - InputStyle.innerFieldMargin * 2
        height: InputStyle.rowHeight

        anchors.top: canvasRoot.top

        bgColor: InputStyle.secondaryBackgroundColor
        fontColor: "white"

        source: InputStyle.infoIcon

        visibleInterval: 10000

        text: qsTr( "Create line to split the selected feature" )
      }

      AutoHideBanner { // TODO: Replace by MapBlurItem later
        id: howtoEditingBanner

        width: parent.width - InputStyle.innerFieldMargin * 2
        height: InputStyle.rowHeight

        anchors.top: canvasRoot.top

        bgColor: InputStyle.secondaryBackgroundColor
        fontColor: "white"

        source: InputStyle.infoIcon

        visibleInterval: 10000

        text: qsTr( "Select some point to start editing the geometry" )
      }

      AutoHideBanner { // TODO: Replace by MapBlurItem later
        id: redrawGeometryBanner

        width: parent.width - InputStyle.innerFieldMargin * 2
        height: InputStyle.rowHeight

        anchors.top: canvasRoot.top

        bgColor: InputStyle.secondaryBackgroundColor
        fontColor: "white"

        source: InputStyle.infoIcon

        visibleInterval: 10000

        text: qsTr( "Record new geometry for the feature" )
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
            __notificationModel.add( qsTr( "GPS currently unavailable" ), 5, NotificationType.Error, NotificationType.None )
            return
          }

          if ( recordingToolsLoader.active ) {
            recordingToolsLoader.item.recordingMapTool.centeredToGPS = true
          }

          mapSettings.setCenter( mapPositionSource.mapPosition )
        }

        onClickAndHold: {
          // start/stop the streaming mode
          if ( recordingToolsLoader.active ) {
            if ( recordingToolsLoader.item.recordingMapTool.recordingType === RecordingMapTool.Manual )
            {
              recordingToolsLoader.item.recordingMapTool.recordingType = RecordingMapTool.StreamMode

              // add first point immediately
              recordingToolsLoader.item.recordingMapTool.addPoint( crosshair.recordPoint )
              root.map.mapSettings.setCenter( mapPositionSource.mapPosition )
            }
            else
            {
              recordingToolsLoader.item.recordingMapTool.recordingType = RecordingMapTool.Manual
            }
          }
        }
      }

      MMMapButton {
        id: backButton

        anchors {
          top: parent.top
          topMargin: __style.mapButtonsMargin
          left: parent.left
          leftMargin: __style.mapButtonsMargin
        }

        iconSource: __style.backIcon

        visible: internal.isInRecordState || root.state === "split"

        onClicked: {
          if ( root.state === "edit" || root.state === "record" || root.state === "recordInLayer") {
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

        anchors {
          top: parent.top
          topMargin: __style.mapButtonsMargin
          left: backButton.right
          leftMargin: __style.mapButtonsMargin
        }

        width: Math.min( parent.width - backButton.width - ( 3 * __style.mapButtonsMargin ), 500 * __dp )

        text: __activeLayer.layerName
        leftIconSource: __inputUtils.loadIconFromLayer( __activeLayer.layer )

        visible: root.state === "record"

        onClicked: activeLayerPanel.openPanel()
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

      SplittingFailedDialog {
        id: splittingFailedDialog
      }

      MMMapLabel { // accuracy button
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

      ActiveLayerPanel {
        id: activeLayerPanel

        height: window.height/2
        width: window.width
        edge: Qt.BottomEdge
      }

      Connections {
        target: mapCanvas.mapSettings
        function onExtentChanged() {
          scaleBar.visible = true
        }
      }

      Component {
        id: recordingToolsComponent

        RecordingTools {
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

        StakeoutTools {
          anchors.fill: parent

          map: mapCanvas
          mapExtentOffset: root.mapExtentOffset

          target: internal.stakeoutTarget
        }
      }

      Component {
        id: splittingToolsComponent

        SplittingTools {
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

  function redraw( featurepair ) {
    __activeProject.setActiveLayer( featurepair.layer )
    root.centerToPair( featurepair )
    redrawGeometryBanner.show()

    // clear feature geometry
    internal.featurePairToEdit = __inputUtils.changeFeaturePairGeometry( featurepair, __inputUtils.emptyGeometry() )

    state = "edit"
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
      showMessage( qsTr( "GPS currently unavailable." ) )
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
