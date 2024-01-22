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
import "../components"

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

  property alias recordingMapTool: mapTool

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
    recordingIntervalType: __appSettings.intervalType

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
          root.map.jumpTo( /*crosshair.screenPoint,*/ root.map.mapSettings.coordinateToScreen( newCenter ) )
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
    realGeometry: __inputUtils.transformGeometryToMapWithLayer( mapTool.recordedGeometry, __activeLayer.vectorLayer, root.map.mapSettings )

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
    geometry: __inputUtils.transformGeometryToMapWithLayer( mapTool.recordedGeometry, __activeLayer.vectorLayer, root.map.mapSettings )

    lineBorderWidth: 0
  }

  Highlight {
    id: handlesHighlight

    height: root.map.height
    width: root.map.width

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.transformGeometryToMapWithLayer( mapTool.handles, __activeLayer.vectorLayer, root.map.mapSettings )

    lineStrokeStyle: ShapePath.DashLine
    lineWidth: Highlight.LineWidths.Narrow
  }

  Highlight {
    id: guideline

    height: root.map.height
    width: root.map.width

    lineWidth: Highlight.LineWidths.Narrow
    lineStrokeStyle: ShapePath.DashLine

    mapSettings: root.map.mapSettings
    geometry: guidelineController.guidelineGeometry
  }

  Highlight {
    id: midSegmentsHighlight

    height: root.map.height
    width: root.map.width

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.transformGeometryToMapWithLayer( mapTool.midPoints, __activeLayer.vectorLayer, root.map.mapSettings )

    markerType: Highlight.MarkerTypes.Circle
    markerBorderColor: __style.grapeColor
  }

  Highlight {
    id: existingVerticesHighlight

    height: root.map.height
    width: root.map.width

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.transformGeometryToMapWithLayer( mapTool.existingVertices, __activeLayer.vectorLayer, root.map.mapSettings )

    markerType: Highlight.MarkerTypes.Circle
    markerSize: Highlight.MarkerSizes.Bigger
  }

  Crosshair {
    id: crosshair

    anchors.fill: parent

    visible: mapTool.state !== RecordingMapTool.View

    qgsProject: __activeProject.qgsProject
    mapSettings: root.map.mapSettings
    shouldUseSnapping: !mapTool.isUsingPosition
  }

  MMToolbar {

    y: parent.height

    width: parent.width

    ObjectModel {
      id: polygonToolbarButtons

      MMToolbarButton {
        text: qsTr( "Undo" )
        iconSource: __style.undoIcon
        onClicked: mapTool.undo()
      }

      MMToolbarButton {
        text: qsTr( "Remove" )
        iconSource: __style.minusIcon
        onClicked: mapTool.removePoint()
      }

      MMToolbarButton {
        text: mapTool.state === RecordingMapTool.Grab ? qsTr( "Release" ) : qsTr( "Add" )
        iconSource: __style.addIcon
        onClicked: {
          if ( mapTool.state === RecordingMapTool.Grab ) {
            mapTool.releaseVertex( crosshair.recordPoint )
          }
          else {
            mapTool.addPoint( crosshair.recordPoint )
          }
        }
      }

      MMToolbarButton {
        text: qsTr( "Record" );
        iconSource: __style.doneIcon;
        type: MMToolbarButton.Button.Emphasized;
        onClicked: {
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
    }

    ObjectModel {
      id: pointToolbarButtons

      MMToolbarLongButton {
        text: qsTr( "Record" );
        iconSource: __style.deleteIcon;
        onClicked: {
          mapTool.addPoint( crosshair.recordPoint )
          let pair = mapTool.getFeatureLayerPair()
          root.done( pair )
        }
      }
    }

    model: {
      let pointLayerSelected = __inputUtils.isPointLayer( __activeLayer.vectorLayer )
      let isMultiPartLayerSelected = __inputUtils.isMultiPartLayer( __activeLayer.vectorLayer )

      if ( pointLayerSelected && !isMultiPartLayerSelected ) {
        return pointToolbarButtons
      }
      return polygonToolbarButtons
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
