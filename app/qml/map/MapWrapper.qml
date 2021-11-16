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
import "../"

Item {
  id: root
  readonly property alias compass: _compass
  readonly property alias digitizingController: _digitizingController
  property var featurePairToEdit // we are editing geometry of this feature layer pair
  readonly property alias gpsIndicatorColor: _gpsState.indicatorColor
  property bool isInRecordState
  readonly property alias mapSettings: _map.mapSettings
  readonly property alias positionKit: _positionKit
  property real previewPanelHeight
  property var targetLayerToUse // layer used in digitizing when recording in specific layer

  state: "view"

  function addRecordPoint() {
    let recordedPoint = findRecordedPoint();
    let isPointGeometry = _digitizingController.hasPointGeometry(_digitizingController.layer);
    let isUsingGPS = _digitizingController.useGpsPoint;
    let hasAssignedValidPair = root.featurePairToEdit && root.featurePairToEdit.valid;
    if (root.state === "recordFeature" || root.state === "recordInLayerFeature") {
      if (isPointGeometry) {
        let newPair = _digitizingController.pointFeatureFromPoint(recordedPoint, isUsingGPS);
        processRecordedPair(newPair);
        return;
      }
      if (!_digitizingController.recording)
        _digitizingController.startRecording();
      _digitizingController.addRecordPoint(recordedPoint, isUsingGPS);
    } else if (root.state === "editGeometry") {
      if (isPointGeometry && hasAssignedValidPair) {
        let changed = _digitizingController.changePointGeometry(root.featurePairToEdit, recordedPoint, isUsingGPS);
        _digitizingHighlight.featureLayerPair = changed;
        root.editingGeometryFinished(changed);
        return;
      }
    }
  }
  function centerToPair(pair, considerFormPreview = false) {
    if (considerFormPreview)
      var previewPanelHeightRatio = previewPanelHeight / _map.height;
    else
      previewPanelHeightRatio = 0;
    __inputUtils.setExtentToFeature(pair, _map.mapSettings, previewPanelHeightRatio);
  }
  function centerToPosition() {
    if (_positionKit.hasPosition) {
      _map.mapSettings.setCenter(_positionKit.projectedPosition);
      _digitizingController.useGpsPoint = true;
    } else {
      showMessage(qsTr("GPS currently unavailable.%1Try to allow GPS Location in your device settings.").arg("\n"));
    }
  }
  function clear() {
    // clear all previous references to old project (if we don't clear references to the previous project,
    // highlights may end up with dangling pointers to map layers and cause crashes)
    _highlightIdentified.featureLayerPair = null;
    _digitizingHighlight.featureLayerPair = null;
  }
  function createFeature(layer) {
    // creates feature without geometry in layer
    return _digitizingController.featureWithoutGeometry(layer);
  }
  signal editingGeometryCanceled
  signal editingGeometryFinished(var pair)
  signal editingGeometryStarted
  signal featureIdentified(var pair)

  //! Returns point from GPS (WGS84) or center screen point in map CRS
  function findRecordedPoint() {
    return _digitizingController.useGpsPoint ? _positionKit.position : // WGS84
    _map.mapSettings.screenToCoordinate(_crosshair.center); // map CRS
  }
  function hideHighlight() {
    _highlightIdentified.visible = false;
    _digitizingHighlight.visible = false;
  }
  function highlightPair(pair) {
    _highlightIdentified.featureLayerPair = pair;
    _highlightIdentified.visible = true;
  }
  function isPositionOutOfExtent() {
    let border = InputStyle.mapOutOfExtentBorder;
    return ((_positionKit.screenPosition.x < border) || (_positionKit.screenPosition.y < border) || (_positionKit.screenPosition.x > _map.width - border) || (_positionKit.screenPosition.y > _map.height - border));
  }
  signal nothingIdentified
  signal notify(string message)
  function processRecordedPair(pair) {
    if (_digitizingController.isPairValid(pair)) {
      if (root.state === "recordFeature") {
        root.recordingFinished(pair);
      } else {
        root.recordInLayerFeatureFinished(pair);
      }
    } else {
      root.notify(qsTr("Recorded feature is not valid"));
      if (root.state === "recordFeature") {
        root.recordingCanceled();
      } else {
        root.recordInLayerFeatureCanceled();
      }
    }
    root.state = "view";
  }
  signal recordInLayerFeatureCanceled
  signal recordInLayerFeatureFinished(var pair)
  signal recordInLayerFeatureStarted
  signal recordingCanceled
  signal recordingFinished(var pair)
  signal recordingStarted
  function updatePosition() {
    let autoCenterDuringRecording = _digitizingController.useGpsPoint && root.isInRecordState;
    let autoCenterDuringViewing = !root.isInRecordState && __appSettings.autoCenterMapChecked && isPositionOutOfExtent();
    if (autoCenterDuringRecording || autoCenterDuringViewing) {
      let useGpsPoint = _digitizingController.useGpsPoint;
      centerToPosition();
    }
    _digitizingHighlight.positionChanged();
  }

  onStateChanged: {
    switch (state) {
    case "recordFeature":
      {
        root.centerToPosition();
        break;
      }
    case "recordInLayerFeature":
      {
        __loader.setActiveLayer(root.targetLayerToUse);
        root.recordInLayerFeatureStarted();
        break;
      }
    case "editGeometry":
      {
        __loader.setActiveLayer(root.featurePairToEdit.layer);
        _digitizingHighlight.featureLayerPair = root.featurePairToEdit;
        _digitizingHighlight.visible = true;
        root.editingGeometryStarted();
        break;
      }
    case "view":
      {
        if (_digitizingHighlight.visible)
          _digitizingHighlight.visible = false;
        if (_highlightIdentified.visible)
          _highlightIdentified.visible = false;
        if (_digitizingController.recording)
          _digitizingController.stopRecording();
        break;
      }
    case "inactive":
      {
        break;
      }
    }
  }

  Rectangle {
    anchors.fill: parent
    // background
    color: InputStyle.clrPanelMain
  }
  QgsQuick.MapCanvas {
    id: _map
    height: root.height
    mapSettings.project: __loader.project
    visible: root.state !== "inactive"
    width: root.width

    onClicked: {
      if (!root.isInRecordState) {
        let screenPoint = Qt.point(mouse.x, mouse.y);
        let pair = _identifyKit.identifyOne(screenPoint);
        if (pair.valid) {
          centerToPair(pair, true);
          highlightPair(pair);
          root.featureIdentified(pair);
        } else {
          _highlightIdentified.featureLayerPair = null;
          _highlightIdentified.visible = false;
          root.nothingIdentified();
        }
      }
    }
    onIsRenderingChanged: _loadingIndicator.visible = isRendering

    IdentifyKit {
      id: _identifyKit
      identifyMode: IdentifyKit.TopDownAll
      mapSettings: _map.mapSettings
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
  Compass {
    id: _compass
  }
  PositionMarker {
    id: _positionMarker
    compass: _compass
    positionKit: _positionKit
  }
  StateGroup {
    id: _gpsState
    property color indicatorColor: InputStyle.softRed

    states: [
      State {
        name: "good"
        when: (_positionKit.accuracy > 0) && (_positionKit.accuracy <= __appSettings.gpsAccuracyTolerance)

        PropertyChanges {
          indicatorColor: InputStyle.softGreen
          target: _gpsState
        }
      },
      State {
        name: "low" // below accuracy tolerance
        when: (_positionKit.accuracy > 0) && (_positionKit.accuracy > __appSettings.gpsAccuracyTolerance)

        PropertyChanges {
          indicatorColor: InputStyle.softOrange
          target: _gpsState
        }
      },
      State {
        name: "unavailable"
        when: _positionKit.accuracy <= 0

        PropertyChanges {
          indicatorColor: InputStyle.softRed
          target: _gpsState
        }
      }
    ]
  }
  LoadingIndicator {
    id: _loadingIndicator
    height: InputStyle.mapLoadingIndicatorHeight
    visible: root.state !== "inactive"
    width: _map.width
  }
  ScaleBar {
    id: _scaleBar
    anchors.horizontalCenter: parent.horizontalCenter
    height: InputStyle.scaleBarHeight
    mapSettings: _map.mapSettings
    preferredWidth: Math.min(window.width, 180 * QgsQuick.Utils.dp)
    y: _map.height - height - InputStyle.panelMargin
  }
  Highlight {
    id: _highlightIdentified
    property bool hasPolygon: featureLayerPair !== null ? _digitizingController.hasPolygonGeometry(featureLayerPair.layer) : false

    anchors.fill: _map
    fillColor: InputStyle.highlightFillColor
    lineColor: InputStyle.highlightLineColor
    lineWidth: InputStyle.highlightLineWidth
    mapSettings: _map.mapSettings
    markerAnchorY: InputStyle.mapMarkerAnchorY
    markerHeight: InputStyle.mapMarkerHeight
    markerImageSource: InputStyle.mapMarkerIcon
    markerType: "image"
    markerWidth: InputStyle.mapMarkerWidth
    outlineColor: InputStyle.highlighOutlineColor
    outlinePenWidth: InputStyle.highlighOutlinePenWidth
  }
  DigitizingController {
    id: _digitizingController
    layer: __activeLayer.vectorLayer
    lineRecordingInterval: __appSettings.lineRecordingInterval
    mapSettings: _map.mapSettings
    positionKit: _positionKit
    variablesManager: __variablesManager

    onFeatureLayerPairChanged: {
      if (recording) {
        _digitizingHighlight.visible = true;
        _digitizingHighlight.featureLayerPair = featureLayerPair;
      }
    }
    onRecordingChanged: __loader.recording = recording
    onUseGpsPointChanged: __variablesManager.useGpsPoint = _digitizingController.useGpsPoint
  }
  RecordCrosshair {
    id: _crosshair
    height: root.height
    visible: _digitizingController.manualRecording && root.isInRecordState
    width: root.width
  }
  Highlight {
    id: _digitizingHighlight
    anchors.fill: _map
    fillColor: _highlightIdentified.fillColor
    guideLineAllowed: _digitizingController.manualRecording && root.isInRecordState
    hasPolygon: featureLayerPair !== null ? _digitizingController.hasPolygonGeometry(featureLayerPair.layer) : false
    lineColor: _highlightIdentified.lineColor
    lineWidth: _highlightIdentified.lineWidth
    mapSettings: _map.mapSettings
    markerAnchorY: _highlightIdentified.markerAnchorY
    markerHeight: _highlightIdentified.markerHeight
    markerImageSource: _highlightIdentified.markerImageSource
    markerType: _highlightIdentified.markerType
    markerWidth: _highlightIdentified.markerWidth
    outlineColor: _highlightIdentified.outlineColor
    outlinePenWidth: _highlightIdentified.outlinePenWidth
    recordingInProgress: _digitizingController.recording
  }
  Banner {
    id: _gpsAccuracyBanner
    property bool shouldShowAccuracyWarning: {
      let isLowAccuracy = _gpsState.state === "low" || _gpsState.state === "unavailable";
      let isBannerAllowed = __appSettings.gpsAccuracyWarning;
      let isRecording = root.isInRecordState;
      let isUsingPosition = _digitizingController.useGpsPoint || !_digitizingController.manualRecording;
      let isGpsWorking = _positionKit.hasPosition;
      return isLowAccuracy && isBannerAllowed && isRecording && isGpsWorking && isUsingPosition;
    }

    height: InputStyle.rowHeight * 2
    link: __inputHelp.gpsAccuracyHelpLink
    showWarning: shouldShowAccuracyWarning
    text: qsTr("Low GPS position accuracy (%1 m)<br><br>Please make sure you have good view of the sky.").arg(__inputUtils.formatNumber(_positionKit.accuracy))
    width: parent.width - _gpsAccuracyBanner.anchors.margins * 2
  }
  ActiveLayerPanel {
    id: _activeLayerPanel
    edge: Qt.BottomEdge
    height: window.height / 2
    width: window.width

    onActiveLayerChangeRequested: __loader.setActiveLayer(__recordingLayersModel.layerFromLayerId(layerId))
  }
  RecordToolbar {
    id: _recordToolbar
    activeVectorLayer: __activeLayer.vectorLayer
    extraPanelVisible: root.state === "recordFeature"
    gpsIndicatorColor: _gpsState.indicatorColor
    height: InputStyle.rowHeightHeader + ((extraPanelVisible) ? extraPanelHeight : 0)
    manualRecording: _digitizingController.manualRecording
    pointLayerSelected: __activeLayer.vectorLayer ? _digitizingController.hasPointGeometry(activeVectorLayer) : false
    visible: root.isInRecordState
    width: parent.width
    y: extraPanelVisible ? parent.height - extraPanelHeight : parent.height

    onAddClicked: root.addRecordPoint()
    onCancelClicked: {
      if (root.state === "recordFeature")
        root.recordingCanceled();
      else if (root.state === "editGeometry")
        root.editingGeometryCanceled();
      else if (root.state === "recordInLayerFeature")
        root.recordInLayerFeatureCanceled();
      root.state = "view";
    }
    onGpsSwitchClicked: {
      if (_gpsState.state === "unavailable") {
        showMessage(qsTr("GPS currently unavailable.%1Try to allow GPS Location in your device settings.").arg("\n"));
        return;
      }
      _map.mapSettings.setCenter(_positionKit.projectedPosition);
      _digitizingController.useGpsPoint = true;
    }
    onLayerLabelClicked: _activeLayerPanel.openPanel()
    onManualRecordingClicked: {
      _digitizingController.manualRecording = !_digitizingController.manualRecording;
      if (!_digitizingController.manualRecording && root.isInRecordState) {
        _digitizingController.startRecording();
        _digitizingController.useGpsPoint = true;
        updatePosition();
        root.addRecordPoint(); // record point immediately after turning on the streaming mode
      }
    }
    onRemovePointClicked: _digitizingController.removeLastPoint()
    onStopRecordingClicked: {
      if (root.state === "recordFeature" || root.state === "recordInLayerFeature") {
        var newPair = _digitizingController.lineOrPolygonFeature();
        root.processRecordedPair(newPair);
      }
    }

    // reset manualRecording after opening
    onVisibleChanged: {
      if (visible)
        _digitizingController.manualRecording = true;
      if (_gpsAccuracyBanner.showWarning) {
        _gpsAccuracyBanner.state = visible ? "show" : "fade";
      }
    }
  }
  Connections {
    target: _map.mapSettings

    onExtentChanged: {
      _digitizingController.useGpsPoint = false;
      _scaleBar.visible = true;
    }
  }

  states: [
    State {
      name: "view"

      PropertyChanges {
        isInRecordState: false
        target: root
      }
    },
    State {
      name: "recordFeature"

      PropertyChanges {
        isInRecordState: true
        target: root
      }
    },
    State {
      // recording feature in specific layer without option to change the digitized layer.
      // can be used to create linked features in relations, value relations and browse data
      name: "recordInLayerFeature"

      PropertyChanges {
        isInRecordState: true
        target: root
      }
    },
    State {
      name: "editGeometry" // of existing feature

      PropertyChanges {
        isInRecordState: true
        target: root
      }
    },
    State {
      name: "inactive" // covered by other element

      PropertyChanges {
        isInRecordState: false
        target: root
      }
    }
  ]
}
