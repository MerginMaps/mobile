/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Shapes 1.14

import QgsQuick 0.1
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

  property var initialGeometry
  property bool centerToGPSOnStartup: false

  signal canceled()
  signal done( var geometry )

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

  RecordingMapTool {
    id: mapTool

    property bool isUsingPosition: centeredToGPS || mapTool.recordingType == RecordingMapTool.StreamMode

    centeredToGPS: false
    mapSettings: root.map.mapSettings

    recordingType: RecordingMapTool.Manual
    recordingInterval: __appSettings.lineRecordingInterval

    positionKit: __positionKit
    layer: __activeLayer.vectorLayer

    initialGeometry: root.initialGeometry ? root.initialGeometry : __inputUtils.emptyGeometry()

    // Bind variables manager to know if we are centered to GPS or not when evaluating position variables
    onIsUsingPositionChanged: __variablesManager.useGpsPoint = isUsingPosition
  }

  GuidelineController {
    id: guidelineController

    mapSettings: root.map.mapSettings
    crosshairPosition: crosshair.screenPoint
    realGeometry: __inputUtils.convertGeometryToMapCRS( mapTool.recordedGeometry, __activeLayer.vectorLayer, root.map.mapSettings )
  }

  Highlight {
    id: guideline

    height: root.map.height
    width: root.map.width

    lineColor: InputStyle.guidelineColor

    mapSettings: root.map.mapSettings
    geometry: guidelineController.guidelineGeometry
  }

  Highlight {
    id: highlight

    height: root.map.height
    width: root.map.width

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.convertGeometryToMapCRS( mapTool.recordedGeometry, __activeLayer.vectorLayer, root.map.mapSettings )
  }

  Highlight {
    id: handlesHighlight

    height: root.map.height
    width: root.map.width

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.convertGeometryToMapCRS( mapTool.handles, __activeLayer.vectorLayer, root.map.mapSettings )

    lineColor: InputStyle.guidelineColor
    lineStrokeStyle: ShapePath.DashLine
  }

  Highlight {
    id: midSegmentsHighlight

    height: root.map.height
    width: root.map.width

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.convertGeometryToMapCRS( mapTool.midPoints, __activeLayer.vectorLayer, root.map.mapSettings )

    markerType: "circleWithIcon"
    markerColor: InputStyle.mapMarkerMidVertexColor
    markerCircleSize: InputStyle.mapMarkerMidVertexSize
    markerCircleIconSource: InputStyle.plusIcon
  }

  Highlight {
    id: existingVerticesHighlight

    height: root.map.height
    width: root.map.width

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.convertGeometryToMapCRS( mapTool.existingVertices, __activeLayer.vectorLayer, root.map.mapSettings )

    markerType: "circleWithIcon"
    markerColor: InputStyle.mapMarkerExistingVertexColor
    markerCircleSize: InputStyle.mapMarkerExistingVertexSize
    markerCircleIconSource: InputStyle.mapMarkerMoveIcon
  }

  Crosshair {
    id: crosshair

    anchors.fill: parent

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
    pointLayerSelected: __inputUtils.isPointLayer( __activeLayer.vectorLayer )

    manualRecording: mapTool.recordingType === RecordingMapTool.Manual

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
        // finish recording
        root.done( mapTool.recordedGeometry )
      }
    }

    onReleaseClicked: {
      // TODO
    }

    onRemovePointClicked: mapTool.removePoint()

    onDoneClicked: {
      if ( mapTool.hasValidGeometry() )
      {
        root.done( mapTool.recordedGeometry )
      }
      else
      {
        showMessage( qsTr( "You need to add at least %1 points." ).arg( __inputUtils.isLineLayer( mapTool.layer ) ? 2 : 3 ) )
      }
    }

    onCancelClicked: root.canceled()
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
}
