/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14

import QgsQuick 0.1 as QgsQuick
import lc 1.0

import ".."

Item {
  id: root

  property var featurePairToEdit // we are editing geometry of this feature layer pair
  property real previewPanelHeight

  readonly property alias gpsIndicatorColor: _gpsState.indicatorColor
  readonly property alias positionKit: _positionKit
  readonly property alias digitizingController: _digitizingController
  readonly property alias mapSettings: _map.mapSettings
  readonly property alias compass: _compass

  property bool isInRecordState

  signal featureIdentified( var pair )
  signal nothingIdentified()

  signal recordingStarted()
  signal recordingFinished( var pair )
  signal recordingCanceled()

  signal editingStarted()
  signal editingFinished( var pair )

  signal addingGeometryStarted()
  signal addingGeometryFinished( var pair )

  signal notify( string message )

  function centerToPair( pair, considerFormPreview = false ) {
    if ( considerFormPreview )
      var previewPanelHeightRatio = previewPanelHeight / _map.height
    else
      previewPanelHeightRatio = 0

    __inputUtils.setExtentToFeature( pair, _map.mapSettings, previewPanelHeightRatio )
  }

  function highlightPair( pair ) {
    _highlightIdentified.featureLayerPair = pair
    _highlightIdentified.visible = true
  }

  function centerToPosition() {
    if ( _positionKit.hasPosition ) {
      _map.mapSettings.setCenter( _positionKit.projectedPosition )
      _digitizingController.useGpsPoint = true
    }
    else {
      showMessage( qsTr( "GPS currently unavailable.%1Try to allow GPS Location in your device settings." ).arg( "\n" ) )
    }
  }

  function hideHighlight() {
    _highlightIdentified.visible = false
    _digitizingHighlight.visible = false
  }

  function createFeature( layer ) {
    // creates feature without geometry in layer
    return _digitizingController.featureWithoutGeometry( layer )
  }

  //! Returns point from GPS (WGS84) or center screen point in map CRS
  function findRecordedPoint() {
    return _digitizingController.useGpsPoint ?
          _positionKit.position : // WGS84
          _map.mapSettings.screenToCoordinate( _crosshair.center ) // map CRS
  }

  function processRecordedPair( pair ) {
    if ( _digitizingController.isPairValid( pair ) ) {
      root.recordingFinished( pair )
    }
    else {
      root.notify( qsTr( "Recorded feature is not valid" ) )
      root.recordingCanceled()
    }
    root.state = "View"
  }

  function addRecordPoint() {
    let recordedPoint = findRecordedPoint()

    let isPointGeometry = _digitizingController.hasPointGeometry( _digitizingController.layer )
    let isUsingGPS = _digitizingController.useGpsPoint
    let hasAssignedValidPair = root.featurePairToEdit && root.featurePairToEdit.valid

    if ( root.state === "RecordFeature" ) {
      if ( isPointGeometry ) {
        let newPair = _digitizingController.pointFeatureFromPoint( recordedPoint, isUsingGPS )
        processRecordedPair( newPair )
        return
      }

      if ( !_digitizingController.recording )
        _digitizingController.startRecording()

      _digitizingController.addRecordPoint( recordedPoint, isUsingGPS )
    }
    else if ( root.state === "EditGeometry" ) {
      if ( isPointGeometry && hasAssignedValidPair ) {
        let changed = _digitizingController.changePointGeometry( root.featurePairToEdit, recordedPoint, isUsingGPS )
        root.editingFinished( changed )
        return
      }
    }
    else if ( root.state === "AddGeometry" ) {
      if ( isPointGeometry ) {
        if ( !hasAssignedValidPair ) return

        let changed = _digitizingController.changePointGeometry( root.featurePairToEdit, recordedPoint, isUsingGPS )
        root.addingGeometryFinished( changed )
        return
      }

      // adding line/polygon geometry
      _digitizingController.addRecordPoint( recordedPoint, isUsingGPS )
    }
  }

  function isPositionOutOfExtent() {
    let border = InputStyle.mapOutOfExtentBorder
    return ( ( _positionKit.screenPosition.x < border ) ||
            ( _positionKit.screenPosition.y < border ) ||
            ( _positionKit.screenPosition.x > _map.width - border ) ||
            ( _positionKit.screenPosition.y > _map.height - border )
            )
  }

  function updatePosition() {
    let autoCenterDuringRecording = _digitizingController.useGpsPoint && root.isInRecordState
    let autoCenterDuringViewing = !root.isInRecordState && __appSettings.autoCenterMapChecked && isPositionOutOfExtent()

    if ( autoCenterDuringRecording || autoCenterDuringViewing ) {
      let useGpsPoint = _digitizingController.useGpsPoint
      centerToPosition()
    }

    _digitizingHighlight.positionChanged()
  }

  function clear() {
    // clear all previous references to old project (if we don't clear references to the previous project,
    // highlights may end up with dangling pointers to map layers and cause crashes)
    _highlightIdentified.featureLayerPair = null
    _digitizingHighlight.featureLayerPair = null
  }

  states: [
    State {
      name: "View"
      PropertyChanges {
        target: root
        isInRecordState: false
      }
    },
    State {
      name: "RecordFeature"
      PropertyChanges {
        target: root
        isInRecordState: true
      }
    },
    State {
      name: "EditGeometry" // of existing feature
      PropertyChanges {
        target: root
        isInRecordState: true
      }
    },
    State {
      name: "AddGeometry" // to existing feature
      PropertyChanges {
        target: root
        isInRecordState: true
      }
    },
    State {
      name: "Inactive" // covered by other element
      PropertyChanges {
        target: root
        isInRecordState: false
      }
    }
  ]

  onStateChanged: {
    switch ( state ) {
      case "RecordFeature": {
        root.centerToPosition()
        break
      }
      case "AddGeometry":
      case "EditGeometry": {
        break
      }
      case "View": {
        if ( _digitizingHighlight.visible )
          _digitizingHighlight.visible = false

        if ( _digitizingController.recording )
          _digitizingController.stopRecording()

        break
      }
      case "Inactive": {
        break
      }
    }
  }

  state: "View"

  QgsQuick.MapCanvas {
    id: _map

    height: root.height
    width: root.width
    visible: root.state !== "Inactive"

    mapSettings.project: __loader.project

    IdentifyKit {
      id: _identifyKit

      mapSettings: _map.mapSettings
      identifyMode: IdentifyKit.TopDownAll
    }

    onIsRenderingChanged: _loadingIndicator.visible = isRendering

    onClicked: {
      if ( !root.isInRecordState ) {
        //       mapCanvas.forceActiveFocus()
        let screenPoint = Qt.point( mouse.x, mouse.y )
        let pair = _identifyKit.identifyOne( screenPoint )

        if ( pair.valid ) {
          centerToPair( pair, true )
          highlightPair( pair )
          root.featureIdentified( pair )
        }
        else {
          _highlightIdentified.featureLayerPair = null
          _highlightIdentified.visible = false
          root.nothingIdentified()
        }
      }
    }
  }

  Item {
    anchors.fill: _map
    transform: QgsQuick.MapTransform {
      mapSettings: _map.mapSettings
    }
  }

  PositionKit {
    id: _positionKit

    mapSettings: _map.mapSettings
    simulatePositionLongLatRad: __use_simulated_position ? [17.130032, 48.130725, 0.1] : []
    onScreenPositionChanged: updatePosition()
  }

  Compass { id: _compass }

  PositionMarker {
    id: _positionMarker

    positionKit: _positionKit
    compass: _compass
  }

  StateGroup {
    id: _gpsState

    property color indicatorColor

    states: [
      State {
        name: "Good"
        when: ( _positionKit.accuracy > 0 ) && ( _positionKit.accuracy < __appSettings.gpsAccuracyTolerance )
        PropertyChanges {
          target: _gpsState
          indicatorColor: InputStyle.softGreen
        }
      },
      State {
        name: "Low" // below accuracy tolerance
        when: ( _positionKit.accuracy > 0 ) && ( _positionKit.accuracy > __appSettings.gpsAccuracyTolerance )
        PropertyChanges {
          target: _gpsState
          indicatorColor: InputStyle.softOrange
        }
      },
      State {
        name: "Unavailable"
        when: _positionKit.accuracy <= 0
        PropertyChanges {
          target: _gpsState
          indicatorColor: InputStyle.softRed
        }
      }
    ]
  }

  LoadingIndicator {
    id: _loadingIndicator

    visible: root.state !== "Inactive"
    width: _map.width
    height: InputStyle.mapLoadingIndicatorHeight
  }

  ScaleBar {
    id: _scaleBar

    mapSettings: _map.mapSettings

    y: _map.height - height - InputStyle.panelMargin
    height: InputStyle.scaleBarHeight
    preferredWidth: Math.min( window.width, 180 * QgsQuick.Utils.dp )
    anchors.horizontalCenter: parent.horizontalCenter
  }

  Highlight {
    id: _highlightIdentified

    anchors.fill: _map

    property bool hasPolygon: featureLayerPair !== null ? _digitizingController.hasPolygonGeometry( featureLayerPair.layer ) : false

    mapSettings: _map.mapSettings

    lineColor: InputStyle.highlightLineColor
    lineWidth: InputStyle.highlightLineWidth

    fillColor: InputStyle.highlightFillColor

    outlinePenWidth: InputStyle.highlighOutlinePenWidth
    outlineColor: InputStyle.highlighOutlineColor

    markerType: "image"
    markerImageSource: InputStyle.mapMarkerIcon
    markerWidth: InputStyle.mapMarkerWidth
    markerHeight: InputStyle.mapMarkerHeight
    markerAnchorY: InputStyle.mapMarkerAnchorY
  }

  DigitizingController  {
    id: _digitizingController

    positionKit: _positionKit
    layer: __activeLayer.vectorLayer
    mapSettings: _map.mapSettings

    lineRecordingInterval: __appSettings.lineRecordingInterval
    variablesManager: __variablesManager

    onRecordingChanged: __loader.recording = recording

    onFeatureLayerPairChanged: {
      if ( recording ) {
        _digitizingHighlight.visible = true
        _digitizingHighlight.featureLayerPair = featureLayerPair
      }
    }

    onUseGpsPointChanged: __variablesManager.useGpsPoint = _digitizingController.useGpsPoint
  }

  RecordCrosshair {
    id: _crosshair

    width: root.width
    height: root.height
    visible: _digitizingController.manualRecording && root.isInRecordState
  }

  Highlight {
    id: _digitizingHighlight
    anchors.fill: _map

    hasPolygon: featureLayerPair !== null ? _digitizingController.hasPolygonGeometry( featureLayerPair.layer ) : false

    mapSettings: _map.mapSettings

    lineColor: _highlightIdentified.lineColor
    lineWidth: _highlightIdentified.lineWidth

    fillColor: _highlightIdentified.fillColor

    outlinePenWidth: _highlightIdentified.outlinePenWidth
    outlineColor: _highlightIdentified.outlineColor

    markerType: _highlightIdentified.markerType
    markerImageSource: _highlightIdentified.markerImageSource
    markerWidth: _highlightIdentified.markerWidth
    markerHeight: _highlightIdentified.markerHeight
    markerAnchorY: _highlightIdentified.markerAnchorY
    recordingInProgress: _digitizingController.recording
    guideLineAllowed: _digitizingController.manualRecording && root.isInRecordState
  }

  Banner {
    id: _gpsAccuracyBanner

    property bool shouldShowAccuracyWarning: {
      let isLowAccuracy = _gpsState.state === "Low" || _gpsState.state === "Unavailable"
      let isBannerAllowed = __appSettings.gpsAccuracyWarning
      let isRecording = root.isInRecordState
      let isUsingPosition = _digitizingController.useGpsPoint || !_digitizingController.manualRecording
      let isGpsWorking = _positionKit.hasPosition

      return isLowAccuracy  &&
          isBannerAllowed   &&
          isRecording       &&
          isGpsWorking      &&
          isUsingPosition
    }

    width: parent.width - _gpsAccuracyBanner.anchors.margins * 2
    height: InputStyle.rowHeight * 2

    text: qsTr( "Low GPS position accuracy (%1 m)<br><br>Please make sure you have good view of the sky." )
    .arg( __inputUtils.formatNumber( _positionKit.accuracy ) )
    link: "https://help.inputapp.io/howto/gps_accuracy"

    showWarning: shouldShowAccuracyWarning
  }

  ActiveLayerPanel {
    id: _activeLayerPanel

    height: window.height/2
    width: window.width
    edge: Qt.BottomEdge

    onActiveLayerChangeRequested: __loader.setActiveLayer( __recordingLayersModel.layerFromLayerId( layerId ) )
  }

  RecordToolbar {
    id: _recordToolbar

    width: parent.width
    height: InputStyle.rowHeightHeader + ( ( extraPanelVisible ) ? extraPanelHeight : 0)
    y: extraPanelVisible ? parent.height - extraPanelHeight : parent.height

    visible: root.state === "RecordFeature"

    gpsIndicatorColor: _gpsState.indicatorColor
    activeVectorLayer: __activeLayer.vectorLayer
    manualRecording: _digitizingController.manualRecording
    pointLayerSelected: __activeLayer.vectorLayer ? _digitizingController.hasPointGeometry( activeVectorLayer ) : false

    // reset manualRecording after opening
    onVisibleChanged: {
      if ( visible ) _digitizingController.manualRecording = true
      if ( _gpsAccuracyBanner.showWarning ) {
        _gpsAccuracyBanner.state = visible ? "show" : "fade"
      }
    }

    onAddClicked: root.addRecordPoint()

    onGpsSwitchClicked: {
      if ( _gpsState.state === "Unavailable" ) {
        showMessage( qsTr( "GPS currently unavailable.%1Try to allow GPS Location in your device settings." ).arg( "\n" ) )
        return
      }
      _map.mapSettings.setCenter( _positionKit.projectedPosition )
      _digitizingController.useGpsPoint = true
    }

    onManualRecordingClicked: {
      _digitizingController.manualRecording = !_digitizingController.manualRecording

      if ( !_digitizingController.manualRecording && root.isInRecordState ) {
        _digitizingController.startRecording()
        _digitizingController.useGpsPoint = true

        updatePosition()

        root.addRecordPoint() // record point immediately after turning on the streaming mode
      }
    }

    onCancelClicked: {
      root.state = "View"
      root.recordingCanceled()
    }

    onRemovePointClicked: _digitizingController.removeLastPoint()

    onStopRecordingClicked: {
      if ( root.state === "RecordFeature" ) {
        var newPair = _digitizingController.lineOrPolygonFeature();
        root.processRecordedPair( newPair )
      }
      else if ( root.state === "AddGeometry" ) {

      }
    }

    onLayerLabelClicked: _activeLayerPanel.openPanel()
  }

  Connections {
    target: _map.mapSettings
    onExtentChanged: {
      _digitizingController.useGpsPoint = false
      _scaleBar.visible = true
    }
  }
}
