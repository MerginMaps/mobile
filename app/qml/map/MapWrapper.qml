/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14

import lc 1.0
import QgsQuick 0.1 as QgsQuick

import ".."
import "../components"

Item {
  id: root

  property var featurePairToEdit // we are editing geometry of this feature layer pair
  property var targetLayerToUse // layer used in digitizing when recording in specific layer

  // mapExtentOffset represents a height (or a portion) of canvas which is occupied by some other component
  // like preview panel or stakeout panel. Map extent thus must be calculated regarding to this
  // offset in order to not highlight features in the occupied area, but rather move canvas
  property real mapExtentOffset

  readonly property alias gpsIndicatorColor: _gpsState.indicatorColor
  readonly property alias digitizingController: _digitizingController
  readonly property alias mapSettings: _map.mapSettings
  readonly property alias compass: _compass

  property bool isInRecordState
  property var extentBeforeStakeout

  // Determines if canvas is auto centered to stakeout line
  property bool autoFollowStakeoutPath: true

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

  signal notify( string message )

  signal stakeoutStarted( var pair )
  signal accuracyButtonClicked()

  function centerToPair( pair, considerMapExtentOffset = false ) {
    if ( considerMapExtentOffset )
      var mapExtentOffsetRatio = mapExtentOffset / _map.height
    else
      mapExtentOffsetRatio = 0

    __inputUtils.setExtentToFeature( pair, _map.mapSettings, mapExtentOffsetRatio )
  }

  function highlightPair( pair ) {
    _highlightIdentified.featureLayerPair = pair
    _highlightIdentified.visible = true
  }

  function centerToPosition() {
    if ( __positionKit.hasPosition ) {
      _map.mapSettings.setCenter( _mapPosition.mapPosition )
      _digitizingController.useGpsPoint = true
    }
    else {
      showMessage( qsTr( "GPS currently unavailable." ) )
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
          __positionKit.positionCoordinate : // WGS84
          _map.mapSettings.screenToCoordinate( _crosshair.center ) // map CRS
  }

  function processRecordedPair( pair ) {
    if ( _digitizingController.isPairValid( pair ) ) {
      if ( root.state === "recordFeature" ) {
        root.recordingFinished( pair )
      }
      else {
        root.recordInLayerFeatureFinished( pair )
      }
    }
    else {
      root.notify( qsTr( "Recorded feature is not valid" ) )
      if ( root.state === "recordFeature" ) {
        root.recordingCanceled()
      }
      else {
        root.recordInLayerFeatureCanceled()
      }
    }
    root.state = "view"
  }

  function addRecordPoint() {
    let recordedPoint = findRecordedPoint()

    let isPointGeometry = _digitizingController.hasPointGeometry( _digitizingController.layer )
    let isUsingGPS = _digitizingController.useGpsPoint
    let hasAssignedValidPair = root.featurePairToEdit && root.featurePairToEdit.valid

    if ( root.state === "recordFeature" || root.state === "recordInLayerFeature" ) {
      if ( isPointGeometry ) {
        let newPair = _digitizingController.pointFeatureFromPoint( recordedPoint, isUsingGPS )
        processRecordedPair( newPair )
        return
      }

      if ( !_digitizingController.recording )
        _digitizingController.startRecording()

      _digitizingController.addRecordPoint( recordedPoint, isUsingGPS )
    }
    else if ( root.state === "editGeometry" ) {
      if ( isPointGeometry && hasAssignedValidPair ) {
        let changed = _digitizingController.changePointGeometry( root.featurePairToEdit, recordedPoint, isUsingGPS )
        _digitizingHighlight.featureLayerPair = changed
        root.editingGeometryFinished( changed )
        return
      }
    }
  }

  function isPositionOutOfExtent() {
    let border = InputStyle.mapOutOfExtentBorder
    return ( ( _mapPosition.screenPosition.x < border ) ||
            ( _mapPosition.screenPosition.y < border ) ||
            ( _mapPosition.screenPosition.x > _map.width - border ) ||
            ( _mapPosition.screenPosition.y > _map.height - border )
            )
  }

  function updatePosition() {
    if ( root.isInRecordState && _digitizingController.useGpsPoint )
    {
      centerToPosition()
    }
    else if ( root.state === "stakeout" )
    {
      if ( root.autoFollowStakeoutPath )
      {
        _map.mapSettings.extent = __inputUtils.stakeoutPathExtent( _mapPosition, _stakeoutHighlight.destinationPair, _map.mapSettings, mapExtentOffset )
      }
    }
    else if ( root.state === "view" )
    {
      if ( __appSettings.autoCenterMapChecked && isPositionOutOfExtent() )
      {
        centerToPosition()
      }
    }

    _digitizingHighlight.positionChanged()
  }

  function clear() {
    // clear all previous references to old project (if we don't clear references to the previous project,
    // highlights may end up with dangling pointers to map layers and cause crashes)
    _highlightIdentified.featureLayerPair = null
    _digitizingHighlight.featureLayerPair = null
    _stakeoutHighlight.destinationPair = null
  }

  function stakeout( feature )
  {
    _stakeoutHighlight.destinationPair = feature
    state = "stakeout"
    root.autoFollowStakeoutPath = true
    root.extentBeforeStakeout = _map.mapSettings.extent
    updatePosition()
    stakeoutStarted( feature )
  }

  function stopStakeout()
  {
    // go back to "view" state and highlight the target pair
    let pair = _stakeoutHighlight.destinationPair
    state = "view"

    // return map extent to position it was before starting stakeout
    _map.mapSettings.extent = root.extentBeforeStakeout
    highlightPair( pair )
    _stakeoutHighlight.destinationPair = null
  }

  onAutoFollowStakeoutPathChanged: {
    if ( autoFollowStakeoutPath )
    {
      updatePosition()
    }
  }

  states: [
    State {
      name: "view"
      PropertyChanges { target: root; isInRecordState: false }
    },
    State {
      name: "recordFeature"
      PropertyChanges { target: root; isInRecordState: true }
    },
    State {
      // recording feature in specific layer without option to change the digitized layer.
      // can be used to create linked features in relations, value relations and browse data
      name: "recordInLayerFeature"
      PropertyChanges { target: root; isInRecordState: true }
    },
    State {
      name: "editGeometry" // of existing feature
      PropertyChanges { target: root; isInRecordState: true }
    },
    State {
      name: "stakeout"
      PropertyChanges { target: root; isInRecordState: false }
    },
    State {
      name: "inactive" // covered by other element
      PropertyChanges { target: root; isInRecordState: false }
    }
  ]

  onStateChanged: {
    switch ( state ) {
      case "recordFeature": {
        root.centerToPosition()
        break
      }
      case "recordInLayerFeature": {
        __loader.setActiveLayer( root.targetLayerToUse )
        root.recordInLayerFeatureStarted()
        break
      }
      case "editGeometry": {
        __loader.setActiveLayer( root.featurePairToEdit.layer )
        _digitizingHighlight.featureLayerPair = root.featurePairToEdit
        _digitizingHighlight.visible = true
        root.editingGeometryStarted()
        break
      }
      case "view": {
        if ( _digitizingHighlight.visible )
          _digitizingHighlight.visible = false

        if ( _highlightIdentified.visible )
          _highlightIdentified.visible = false

        if ( _digitizingController.recording )
          _digitizingController.stopRecording()

        break
      }
      case "stakeout": {
        if ( _digitizingHighlight.visible )
          _digitizingHighlight.visible = false

        if ( _highlightIdentified.visible )
          _highlightIdentified.visible = false

        break
      }
      case "inactive": {
        break
      }
    }    
  }

  state: "view"

  Rectangle {
    // background
    color: InputStyle.clrPanelMain
    anchors.fill: parent
  }

  QgsQuick.MapCanvas {
    id: _map

    height: root.height
    width: root.width
    visible: root.state !== "inactive"

    mapSettings.project: __loader.project

    IdentifyKit {
      id: _identifyKit

      mapSettings: _map.mapSettings
      identifyMode: IdentifyKit.TopDownAll
    }

    onIsRenderingChanged: _loadingIndicator.visible = isRendering

    onClicked: {
      if ( root.state === "view" )
      {
        let screenPoint = Qt.point( point.x, point.y )
        let pair = _identifyKit.identifyOne( screenPoint )

        if ( pair.valid )
        {
          centerToPair( pair, true )
          highlightPair( pair )
          root.featureIdentified( pair )
        }
        else
        {
          _highlightIdentified.featureLayerPair = null
          _highlightIdentified.visible = false
          root.nothingIdentified()
        }
      }
    }

    onLongPressed: {
      // Alter position of simulated provider

      if ( __positionKit.positionProvider && __positionKit.positionProvider.id() === "simulated" )
      {
        __positionKit.positionProvider.setPosition( __inputUtils.mapPointToGps( Qt.point( point.x, point.y ), _map.mapSettings ) )
      }
    }

    onUserInteractedWithMap: {
      if ( root.state === "stakeout" )
      {
        root.autoFollowStakeoutPath = false
      }
    }
  }

  MapPosition {
    id: _mapPosition

    mapSettings: _map.mapSettings
    positionKit: __positionKit
    onScreenPositionChanged: updatePosition()
  }

  Compass { id: _compass }

  StakeoutHighlight {
    id: _stakeoutHighlight
    anchors.fill: _map
    visible: root.state === "stakeout"

    mapSettings: _map.mapSettings

    lineColor: InputStyle.highlightLineColor
    lineWidth: InputStyle.highlightLineWidth * 2

    fillColor: InputStyle.highlightFillColor

    outlinePenWidth: InputStyle.highlightOutlinePenWidth
    outlineColor: InputStyle.highlightOutlineColor
  }

  PositionMarker {
    id: _positionMarker

    mapPosition: _mapPosition
    compass: _compass
  }

  StateGroup {
    id: _gpsState

    property color indicatorColor: InputStyle.softRed

    states: [
      State {
        name: "good" // GPS provides position AND horizontal accuracy is below set tolerance (threshold)
        when: __positionKit.hasPosition && __positionKit.horizontalAccuracy > 0 && __positionKit.horizontalAccuracy <= __appSettings.gpsAccuracyTolerance
        PropertyChanges {
          target: _gpsState
          indicatorColor: InputStyle.softGreen
        }
      },
      State {
        name: "low" // below accuracy tolerance OR GPS does not provide horizontal accuracy
        when: __positionKit.hasPosition &&  (__positionKit.horizontalAccuracy < 0 || __positionKit.horizontalAccuracy > __appSettings.gpsAccuracyTolerance )
        PropertyChanges {
          target: _gpsState
          indicatorColor: InputStyle.softOrange
        }
      },
      State {
        name: "unavailable" // GPS does not provide position
        when: !__positionKit.hasPosition
        PropertyChanges {
          target: _gpsState
          indicatorColor: InputStyle.softRed
        }
      }
    ]
  }

  LoadingIndicator {
    id: _loadingIndicator

    visible: root.state !== "inactive"
    width: _map.width
    height: InputStyle.mapLoadingIndicatorHeight
  }

  ScaleBar {
    id: _scaleBar

    mapSettings: _map.mapSettings

    height: InputStyle.scaleBarHeight
    preferredWidth: Math.min( window.width, 180 * __dp )

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: parent.top
    anchors.topMargin: InputStyle.smallGap
  }

  Highlight {
    id: _highlightIdentified

    anchors.fill: _map

    property bool hasPolygon: featureLayerPair !== null ? _digitizingController.hasPolygonGeometry( featureLayerPair.layer ) : false

    mapSettings: _map.mapSettings

    lineColor: InputStyle.highlightLineColor
    lineWidth: InputStyle.highlightLineWidth

    fillColor: InputStyle.highlightFillColor

    outlinePenWidth: InputStyle.highlightOutlinePenWidth
    outlineColor: InputStyle.highlightOutlineColor

    markerType: "image"
    markerImageSource: InputStyle.mapMarkerIcon
    markerWidth: InputStyle.mapMarkerWidth
    markerHeight: InputStyle.mapMarkerHeight
    markerAnchorY: InputStyle.mapMarkerAnchorY
  }

  DigitizingController  {
    id: _digitizingController

    positionKit: __positionKit
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
      let isLowAccuracy = _gpsState.state === "low" || _gpsState.state === "unavailable"
      let isBannerAllowed = __appSettings.gpsAccuracyWarning
      let isRecording = root.isInRecordState
      let isUsingPosition = _digitizingController.useGpsPoint || !_digitizingController.manualRecording
      let isGpsWorking = __positionKit.hasPosition

      return isLowAccuracy  &&
          isBannerAllowed   &&
          isRecording       &&
          isGpsWorking      &&
          isUsingPosition
    }

    width: parent.width - _gpsAccuracyBanner.anchors.margins * 2
    height: InputStyle.rowHeight * 2

    text: qsTr( "Low GPS position accuracy (%1 m)<br><br>Please make sure you have good view of the sky." )
    .arg( __inputUtils.formatNumber( __positionKit.horizontalAccuracy ) )
    link: __inputHelp.gpsAccuracyHelpLink

    showWarning: shouldShowAccuracyWarning
  }

  MapFloatButton {
    id: _accuracyButton

    property int accuracyPrecision: __positionKit.horizontalAccuracy > 1 ? 1 : 2

    onClicked: accuracyButtonClicked()

    maxWidth: parent.width - ( InputStyle.panelMargin * 2 )

    anchors.bottom: root.state === "recordFeature" ? _activeLayerButton.top : parent.bottom
    anchors.bottomMargin: root.mapExtentOffset + InputStyle.smallGap
    anchors.horizontalCenter: parent.horizontalCenter

    visible: {
      if ( root.state === "inactive" )
      {
        return false
      }
      else if ( __positionKit.positionProvider && __positionKit.positionProvider.type() === "external" )
      {
        // for external receivers we want to show gps panel and accuracy button
        // even when the GPS receiver is not sending position data
        return true
      }
      else return ( _gpsState.state !== "unavailable" )
    }

    content: Item {

      implicitWidth: acctext.implicitWidth + indicator.width + InputStyle.tinyGap
      height: parent.height

      anchors.horizontalCenter: parent.horizontalCenter

      Text {
        id: acctext

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
          return __inputUtils.formatNumber( __positionKit.horizontalAccuracy, _accuracyButton.accuracyPrecision ) + " m"
        }
        elide: Text.ElideRight
        wrapMode: Text.NoWrap

        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor

        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
      }

      RoundIndicator {
        id: indicator

        width: parent.height / 4
        height: width
        anchors.left: acctext.right
        anchors.leftMargin: InputStyle.tinyGap
        anchors.topMargin: InputStyle.tinyGap
        anchors.top: parent.top
        color: _gpsState.indicatorColor
      }
    }
  }

  MapFloatButton {
    id: _activeLayerButton

    onClicked: _activeLayerPanel.openPanel()

    maxWidth: parent.width * 0.8

    anchors.bottom: parent.bottom
    anchors.bottomMargin: InputStyle.smallGap
    anchors.horizontalCenter: _accuracyButton.horizontalCenter

    visible: root.state === "recordFeature"

    content: Item {

      implicitWidth: layername.implicitWidth + layericon.width + InputStyle.tinyGap
      height: parent.height

      anchors.horizontalCenter: parent.horizontalCenter

      Symbol {
        id: layericon

        iconSize: parent.height / 2
        source: __loader.loadIconFromLayer( __activeLayer.layer )

        anchors.verticalCenter: parent.verticalCenter
      }

      Text {
        id: layername

        property real maxTextWidth: _activeLayerButton.maxWidth - ( layericon.width + InputStyle.tinyGap + leftPadding ) // used offsets

        text: textmetrics.elidedText
        elide: Text.ElideRight
        wrapMode: Text.NoWrap

        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor

        height: parent.height

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        leftPadding: height / 3 // small gap between layer icon and layer name

        TextMetrics { // element holding metrics about printed text to be able to scale text without binding loops
          id: textmetrics

          font: layername.font
          text: __activeLayer.layerName
          elide: layername.elide
          elideWidth: layername.maxTextWidth
        }

        anchors {
          left: layericon.right
          right: parent.right
          verticalCenter: parent.verticalCenter
        }
      }
    }
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
    height: InputStyle.rowHeightHeader
    y: parent.height

    visible: root.isInRecordState

    gpsIndicatorColor: _gpsState.indicatorColor
    manualRecording: _digitizingController.manualRecording
    pointLayerSelected: __inputUtils.geometryFromLayer( __activeLayer.vectorLayer ) === "point"

    // reset manualRecording after opening
    onVisibleChanged: {
      if ( visible ) _digitizingController.manualRecording = true
      if ( _gpsAccuracyBanner.showWarning ) {
        _gpsAccuracyBanner.state = visible ? "show" : "fade"
      }
    }

    onAddClicked: root.addRecordPoint()

    onGpsSwitchClicked: {
      if ( _gpsState.state === "unavailable" ) {
        showMessage( qsTr( "GPS currently unavailable." ) )
        return
      }
      _map.mapSettings.setCenter( _mapPosition.mapPosition )
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
      if ( root.state === "recordFeature" )
        root.recordingCanceled()
      else if ( root.state === "editGeometry" )
        root.editingGeometryCanceled()
      else if ( root.state === "recordInLayerFeature" )
        root.recordInLayerFeatureCanceled()

      root.state = "view"
    }

    onRemovePointClicked: _digitizingController.removeLastPoint()

    onStopRecordingClicked: {
      if ( root.state === "recordFeature" || root.state === "recordInLayerFeature" ) {
        var newPair = _digitizingController.lineOrPolygonFeature();
        root.processRecordedPair( newPair )
      }
    }
  }

  Connections {
    target: _map.mapSettings
    onExtentChanged: {
      _digitizingController.useGpsPoint = false
      _scaleBar.visible = true
    }
  }
}
