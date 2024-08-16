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

  MMCrosshair { //labeled crosshair
    id: crosshair

    anchors.fill: parent

    visible: true //mapTool.state !== MM.RecordingMapTool.View && mapTool.recordingType !== MM.RecordingMapTool.StreamMode

    qgsProject: __activeProject.qgsProject
    mapSettings: root.map.mapSettings
    shouldUseSnapping: !mapTool.isUsingPosition
    hasLabel: true
    crosshairLabelText: "58.4 m"
  }


  function discardChanges() {
    mapTool.discardChanges()
    root.canceled()
  }

  function hasChanges() {
    return mapTool.hasChanges()
  }
}
