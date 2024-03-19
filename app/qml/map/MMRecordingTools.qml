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

import mm 1.0 as MM

import "../components"
import "./components"

/**
  * RecordingTools is a set of tools that are used during recording/editing of a geometry.
  * These tools can be instantiated just for the time of recording and then destroyed.
  */
Item {
  id: root

  required property MMMapCanvas map
  required property MMPositionMarker positionMarkerComponent

  property alias recordingMapTool: mapTool

  property var activeFeature

  signal canceled()
  signal done( var featureLayerPair )

  function toggleStreaming() {
    if (  recordingMapTool.recordingType === MM.RecordingMapTool.Manual )
    {
      recordingMapTool.recordingType = MM.RecordingMapTool.StreamMode

      // add first point immediately
      recordingMapTool.addPoint( crosshair.recordPoint )
      root.map.mapSettings.setCenter( mapPositionSource.mapPosition )
    }
    else
    {
      recordingMapTool.recordingType = MM.RecordingMapTool.Manual
    }
  }

  MM.RecordingMapTool {
    id: mapTool

    property bool isUsingPosition: mapTool.centeredToGPS || mapTool.recordingType == MM.RecordingMapTool.StreamMode

    mapSettings: root.map.mapSettings

    recordPoint: crosshair.recordPoint

    recordingType: MM.RecordingMapTool.Manual
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

  MM.GuidelineController {
    id: guidelineController

    allowed: mapTool.state !== MM.RecordingMapTool.View

    mapSettings: root.map.mapSettings
    insertPolicy: mapTool.insertPolicy
    crosshairPosition: crosshair.screenPoint
    realGeometry: __inputUtils.transformGeometryToMapWithLayer( mapTool.recordedGeometry, __activeLayer.vectorLayer, root.map.mapSettings )

    activeVertex: mapTool.activeVertex
    activePart: mapTool.activePart
    activeRing: mapTool.activeRing
  }

  MMHighlight {
    id: highlight

    height: root.map.height
    width: root.map.width

    visible: !__inputUtils.isPointLayer(__activeLayer.vectorLayer)

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.transformGeometryToMapWithLayer( mapTool.recordedGeometry, __activeLayer.vectorLayer, root.map.mapSettings )

    lineBorderWidth: 0
  }

  MMHighlight {
    id: handlesHighlight

    height: root.map.height
    width: root.map.width

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.transformGeometryToMapWithLayer( mapTool.handles, __activeLayer.vectorLayer, root.map.mapSettings )

    lineStrokeStyle: ShapePath.DashLine
    lineWidth: MMHighlight.LineWidths.Narrow
  }

  MMHighlight {
    id: guideline

    height: root.map.height
    width: root.map.width

    lineWidth: MMHighlight.LineWidths.Narrow
    lineStrokeStyle: ShapePath.DashLine

    mapSettings: root.map.mapSettings
    geometry: guidelineController.guidelineGeometry
  }

  MMHighlight {
    id: midSegmentsHighlight

    height: root.map.height
    width: root.map.width

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.transformGeometryToMapWithLayer( mapTool.midPoints, __activeLayer.vectorLayer, root.map.mapSettings )

    markerType: MMHighlight.MarkerTypes.Circle
    markerBorderColor: __style.grapeColor
  }

  MMHighlight {
    id: existingVerticesHighlight

    height: root.map.height
    width: root.map.width

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.transformGeometryToMapWithLayer( mapTool.existingVertices, __activeLayer.vectorLayer, root.map.mapSettings )

    markerType: MMHighlight.MarkerTypes.Circle
    markerSize: MMHighlight.MarkerSizes.Bigger
  }

  // Duplicate position marker to be painted on the top of highlights
  MMPositionMarker {
    xPos: positionMarkerComponent.xPos
    yPos: positionMarkerComponent.yPos
    hasDirection: positionMarkerComponent.hasDirection

    direction: positionMarkerComponent.direction
    hasPosition: positionMarkerComponent.hasPosition

    horizontalAccuracy: positionMarkerComponent.horizontalAccuracy
    accuracyRingSize: positionMarkerComponent.accuracyRingSize
  }

  MMCrosshair {
    id: crosshair

    anchors.fill: parent

    visible: mapTool.state !== MM.RecordingMapTool.View

    qgsProject: __activeProject.qgsProject
    mapSettings: root.map.mapSettings
    shouldUseSnapping: !mapTool.isUsingPosition
  }

  MMToolbar {
    y: parent.height

    ObjectModel {
      id: polygonToolbarButtons

      MMToolbarButton {
        text: qsTr( "Undo" )
        iconSource: __style.undoIcon
        onClicked: mapTool.undo()
        enabled: root.recordingMapTool.canUndo
      }

      MMToolbarButton {
        text: qsTr( "Remove" )
        iconSource: __style.minusIcon
        onClicked: mapTool.removePoint()

        enabled: {
           if ( root.recordingMapTool.recordingType !== MM.RecordingMapTool.Manual ) return false;
           if ( root.recordingMapTool.state === MM.RecordingMapTool.View ) return false;
           if ( __inputUtils.isEmptyGeometry( root.recordingMapTool.recordedGeometry ) ) return false;

           return true;
         }
      }

      MMToolbarButton {
        text: qsTr( "Release" )
        visibilityMode: root.recordingMapTool.state === MM.RecordingMapTool.Grab
        iconSource: __style.addIcon
        onClicked: {
          if ( mapTool.state === MM.RecordingMapTool.Grab ) {
            mapTool.releaseVertex( crosshair.recordPoint )
          }
          else {
            mapTool.addPoint( crosshair.recordPoint )
          }
        }
      }

      MMToolbarButton {
        text: qsTr( "Add" )

        visibilityMode: root.recordingMapTool.state === MM.RecordingMapTool.View || root.recordingMapTool.state === MM.RecordingMapTool.Record
        enabled: root.recordingMapTool.recordingType === MM.RecordingMapTool.Manual && root.recordingMapTool.state !== MM.RecordingMapTool.View

        iconSource: __style.addIcon
        onClicked: {
          if ( mapTool.state === MM.RecordingMapTool.Grab ) {
            mapTool.releaseVertex( crosshair.recordPoint )
          }
          else {
            mapTool.addPoint( crosshair.recordPoint )
          }
        }
      }

      MMToolbarButton {
        text: qsTr( "Record" )

        iconSource: __style.doneCircleIcon
        iconColor: __style.grassColor
        onClicked: {
          if ( mapTool.hasValidGeometry() )
          {
            // If we currently grab a point
            if ( mapTool.state === MM.RecordingMapTool.Grab )
            {
              mapTool.releaseVertex( crosshair.recordPoint )
            }

            let pair = mapTool.getFeatureLayerPair()
            root.done( pair )
          }
          else
          {
            __notificationModel.addWarning( __inputUtils.invalidGeometryWarning( mapTool.activeLayer ) )
          }
        }
      }
    }

    ObjectModel {
      id: pointToolbarButtons

      MMToolbarLongButton {
        text: qsTr( "Record" );
        iconSource: __style.doneCircleIcon;
        iconColor: __style.forestColor
        onClicked: {
          if ( mapTool.state === MM.RecordingMapTool.Grab )
          {
            // editing existing point geometry
            mapTool.releaseVertex( crosshair.recordPoint )
          } else
          {
            // recording new point
            mapTool.addPoint( crosshair.recordPoint )
          }

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

  Connections {
    target: map
    function onClicked( point ) {
      let screenPoint = Qt.point( point.x, point.y )

      mapTool.lookForVertex( screenPoint )
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
