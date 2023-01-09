/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Shapes
import Input 0.1
import lc 1.0

import "../"

/**
  * RecordingTools is a set of tools that are used during recording/editing of a geometry.
  * These tools can be instantiated just for the time of recording and then destroyed.
  */
Item {
  id: root

  /*required*/ property var map
  /*required*/ property var gpsState
  /*required*/ property var compass

  property alias gpsBanner: gpsAccuracyBanner

  property bool centerToGPSOnStartup: false
  property var activeFeature

  signal canceled()
  signal done( var featureLayerPair )

  RecordingMapTool {
    id: mapTool

    property bool isUsingPosition: centeredToGPS || mapTool.recordingType == RecordingMapTool.StreamMode

    centeredToGPS: false
    mapSettings: root.map.mapSettings

    recordPoint: crosshair.recordPoint

    recordingType: RecordingMapTool.Manual
    recordingInterval: __appSettings.lineRecordingInterval

    positionKit: __positionKit
    activeLayer: __activeLayer.vectorLayer
    activeFeature: root.activeFeature

    // Bind variables manager to know if we are centered to GPS or not when evaluating position variables
    onIsUsingPositionChanged: __variablesManager.useGpsPoint = isUsingPosition

    onActiveVertexChanged: function( activeVertex ) {
      if ( activeVertex.isValid() )
      {
        // Center to clicked vertex
        let newCenter = mapTool.vertexMapCoors( activeVertex )

        if ( !isNaN( newCenter.x ) && !isNaN( newCenter.y ) )
        {
          root.map.moveTo( /*crosshair.screenPoint,*/ root.map.mapSettings.coordinateToScreen( newCenter ) )
        }
      }
    }
  }

  GuidelineController {
    id: guidelineController

    allowed: mapTool.state !== RecordingMapTool.View

    mapSettings: root.map.mapSettings
    insertPolicy: mapTool.insertPolicy
    crosshairPosition: crosshair.screenPoint
    realGeometry: __inputUtils.convertGeometryToMapCRS( mapTool.recordedGeometry, __activeLayer.vectorLayer, root.map.mapSettings )

    activeVertex: mapTool.activeVertex
    activePart: mapTool.activePart
    activeRing: mapTool.activeRing
  }

  Highlight {
    id: highlight

    height: root.map.height
    width: root.map.width

    visible: !__inputUtils.isPointLayer(__activeLayer.vectorLayer)

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.convertGeometryToMapCRS( mapTool.recordedGeometry, __activeLayer.vectorLayer, root.map.mapSettings )

    lineBorderWidth: 0
  }

  Highlight {
    id: handlesHighlight

    height: root.map.height
    width: root.map.width

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.convertGeometryToMapCRS( mapTool.handles, __activeLayer.vectorLayer, root.map.mapSettings )

    lineStrokeStyle: ShapePath.DashLine
    lineWidth: InputStyle.guidelineWidth
  }

  Highlight {
    id: guideline

    height: root.map.height
    width: root.map.width

    lineWidth: InputStyle.guidelineWidth
    lineStrokeStyle: ShapePath.DashLine

    mapSettings: root.map.mapSettings
    geometry: guidelineController.guidelineGeometry
  }

  Highlight {
    id: midSegmentsHighlight

    height: root.map.height
    width: root.map.width

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.convertGeometryToMapCRS( mapTool.midPoints, __activeLayer.vectorLayer, root.map.mapSettings )

    markerType: "circle"
    markerSize: InputStyle.mapMarkerSize
    markerBorderColor: InputStyle.mapMarkerColor
  }

  Highlight {
    id: existingVerticesHighlight

    height: root.map.height
    width: root.map.width

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.convertGeometryToMapCRS( mapTool.existingVertices, __activeLayer.vectorLayer, root.map.mapSettings )

    markerType: "circle"
    markerSize: InputStyle.mapMarkerSizeBig
  }

  PositionMarker {
    mapPosition: mapPositioning
    compass: root.compass
  }

  Crosshair {
    id: crosshair

    anchors.fill: parent

    visible: mapTool.state !== RecordingMapTool.View

    qgsProject: __activeProject.qgsProject
    mapSettings: root.map.mapSettings
    shouldUseSnapping: !mapTool.isUsingPosition
  }

  RecordingToolbar {
    id: toolbar

    y: parent.height

    width: parent.width
    height: InputStyle.rowHeightHeader

    gpsIndicatorColor: root.gpsState.indicatorColor
    pointLayerSelected: __inputUtils.isPointLayer( __activeLayer.vectorLayer ) && !__inputUtils.isMultiPartLayer( __activeLayer.vectorLayer )

    manualRecording: mapTool.recordingType === RecordingMapTool.Manual

    recordingMapTool: mapTool

    onGpsSwitchClicked: {
      if ( root.gpsState.state === "unavailable" ) {
        showMessage( qsTr( "GPS currently unavailable." ) )
        return
      }

      mapTool.centeredToGPS = true
      root.map.mapSettings.setCenter( mapPositioning.mapPosition )
    }

    onGpsSwithHeld: {
      // start / stop streaming mode
      if ( mapTool.recordingType === RecordingMapTool.Manual )
      {
        mapTool.recordingType = RecordingMapTool.StreamMode

        // add first point immediately
        mapTool.addPoint( crosshair.recordPoint )
        root.map.mapSettings.setCenter( mapPositioning.mapPosition )
      }
      else
      {
        mapTool.recordingType = RecordingMapTool.Manual
      }
    }

    onAddClicked: {
      mapTool.addPoint( crosshair.recordPoint )

      if ( pointLayerSelected )
      {
        let pair = mapTool.getFeatureLayerPair()
        root.done( pair )
      }
    }

    onReleaseClicked: {
      mapTool.releaseVertex( crosshair.recordPoint )
    }

    onRemoveClicked: mapTool.removePoint()

    onUndoClicked: {
      mapTool.undo()
    }

    onDoneClicked: {
      if ( mapTool.hasValidGeometry() )
      {
        // If we currently grab a point
        if ( mapTool.state == RecordingMapTool.Grab )
        {
          mapTool.releaseVertex( crosshair.recordPoint )
        }

        let pair = mapTool.getFeatureLayerPair()
        root.done( pair )
      }
      else
      {
        showMessage( __inputUtils.invalidGeometryWarning( mapTool.activeLayer ) )
      }
    }
  }

  MapPosition {
    id: mapPositioning

    mapSettings: root.map.mapSettings
    positionKit: __positionKit
    onScreenPositionChanged: {
      if ( mapTool.isUsingPosition )
      {
        root.map.mapSettings.setCenter( mapPositioning.mapPosition )
      }
    }
  }

  Banner {
    id: gpsAccuracyBanner

    property bool shouldShowAccuracyWarning: {
      let isLowAccuracy = gpsState.state === "low" || gpsState.state === "unavailable"
      let isBannerAllowed = __appSettings.gpsAccuracyWarning
      let isUsingPosition = mapTool.isUsingPosition
      let isGpsWorking = __positionKit.hasPosition

      return isLowAccuracy  &&
          isBannerAllowed   &&
          isGpsWorking      &&
          isUsingPosition
    }

    width: parent.width - InputStyle.innerFieldMargin * 2
    height: InputStyle.rowHeight * 2

    text: qsTr( "Low GPS position accuracy (%1 m)<br><br>Please make sure you have good view of the sky." )
    .arg( __inputUtils.formatNumber( __positionKit.horizontalAccuracy ) )
    withLink: true
    link: __inputHelp.gpsAccuracyHelpLink

    showBanner: shouldShowAccuracyWarning
  }

  Connections {
    target: map
    function onUserInteractedWithMap() {
      mapTool.centeredToGPS = false
    }

    function onClicked( point ) {
      let screenPoint = Qt.point( point.x, point.y )

      mapTool.lookForVertex( screenPoint )
    }
  }

  Component.onCompleted: {
    if ( root.centerToGPSOnStartup )
    {
      // center to GPS
      if ( root.gpsState.state === "unavailable" ) {
        showMessage( qsTr( "GPS currently unavailable." ) )
        return
      }

      mapTool.centeredToGPS = true
      root.map.mapSettings.setCenter( mapPositioning.mapPosition )
    }
  }

  function discardChanges() {
    mapTool.discardChanges()
    root.canceled()
  }

  function hasChanges() {
    return mapTool.hasChanges()
  }
}
