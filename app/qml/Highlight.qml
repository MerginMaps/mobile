/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.11
import QtQuick.Shapes 1.11

import QgsQuick 0.1 as QgsQuick

Item {
  id: highlight

  signal positionChanged()

  // color for line geometries
  property color lineColor: "black"
  // width for line geometries
  property real lineWidth: 2 * QgsQuick.Utils.dp

  // color for polygon geometries
  property color fillColor: "red"

  // width for outlines of lines and polygons
  property real outlinePenWidth: 1 * QgsQuick.Utils.dp
  // color for outlines of lines and polygons
  property color outlineColor: "black"

  property string markerType: "circle"   // "circle" or "image"
  property color markerColor: "grey"
  property real markerWidth: 30 * QgsQuick.Utils.dp
  property real markerHeight: 30 * QgsQuick.Utils.dp
  property real markerAnchorX: markerWidth/2
  property real markerAnchorY: markerHeight/2
  property url markerImageSource   // e.g. "file:///home/martin/all_the_things.jpg"

  // feature+layer pair which determines what geometry is highlighted
  property var featureLayerPair: null
  property bool hasPolygon: false

  // for transformation of the highlight to the correct location on the map
  property QgsQuick.MapSettings mapSettings

  property bool recordingInProgress: false
  property color helperLineColor: Qt.rgba( 0.67, 0.7, 0.74, 0.5 )

  //
  // internal properties not meant to be modified from outside
  //
  property real markerOffsetY: 14 * QgsQuick.Utils.dp // for circle marker type to be aligned with crosshair
  property real markerCircleSize: 15 * QgsQuick.Utils.dp

  // transform used by line/path
  property QgsQuick.MapTransform mapTransform: QgsQuick.MapTransform {
    mapSettings: highlight.mapSettings
  }

  // properties used by markers (not able to use values directly from mapTransform
  // (no direct access to matrix no mapSettings' visible extent)
  property real mapTransformScale: 1
  property real mapTransformOffsetX: 0
  property real mapTransformOffsetY: 0

  Connections {
      target: mapSettings
      onVisibleExtentChanged: {
          mapTransformScale = __inputUtils.mapSettingsScale(mapSettings)
          mapTransformOffsetX = __inputUtils.mapSettingsOffsetX(mapSettings)
          mapTransformOffsetY = __inputUtils.mapSettingsOffsetY(mapSettings)
      }
  }

  function crosshairPoint() {
    let crosshairCoord = Qt.point( mapCanvas.width/2, mapCanvas.height/2 )
    crosshairCoord = mapCanvas.mapSettings.screenToCoordinate( crosshairCoord )  // map CRS

    return crosshairCoord
  }

  function updateHelperLine() {
    let elements = Object.values( helperPath.pathElements )

    if ( elements.length === 1 && elements[0].x === 0 && elements[0].y === 0 )
      return // if we have not yet added any point, do not draw a line

    if ( hasPolygon && elements.length > 2 ) {
      let firstPoint = elements.pop()
      elements.pop() // last center point
      let centerPoint = crosshairPoint()
      elements.push( componentLineTo.createObject( helperPath, { "x": centerPoint.x, "y": centerPoint.y } ) )
      elements.push( firstPoint )
    }
    else {
      elements.pop() // remove point leading to old crosshair
      let centerPoint = crosshairPoint()
      elements.push( componentLineTo.createObject( helperPath, { "x": centerPoint.x, "y": centerPoint.y } ) )
    }

    helperPath.pathElements = elements
  }

  onFeatureLayerPairChanged: { // highlighting features
    let data = __inputUtils.extractGeometryCoordinates( featureLayerPair, mapSettings )

    let newMarkerItems = []
    let newLineElements = []
    let newPolygonElements = []
    let newHelperLineElements = []

    let geometryType = data[0] // type of geometry - 0: point, 1: linestring, 2: polygon
    let dataStartIndex = ( geometryType === 0 ? 1 : 2 ) // point data starts from index 1, others from index 2

    if ( data.length > dataStartIndex )
    {
      if ( geometryType === 0 ) // point
      {
        newMarkerItems.push( componentMarker.createObject( highlight, { "posX": data[dataStartIndex], "posY": data[dataStartIndex + 1] } ) )
      }
      else // line or polygon
      {
        // place temporary point marker if this is the first point in line / polygon
        if ( recordingInProgress && data.length < dataStartIndex + 3 )
        {
          newMarkerItems.push( componentMarker.createObject( highlight, {
                                                              "posX": data[ dataStartIndex ],
                                                              "posY": data[ dataStartIndex + 1 ],
                                                              "markerType": "circle"
                                                            } ) )
        }

        let objOwner = ( geometryType === 1 ? lineShapePath : polygonShapePath )
        let elements = ( geometryType === 1 ? newLineElements : newPolygonElements )
        let dataLength = data[1] * 2

        // move brush to the first point and start drawing from next point
        elements.push( componentMoveTo.createObject( objOwner, { "x": data[ dataStartIndex ], "y": data[ dataStartIndex + 1 ] } ) )
        for ( let i = dataStartIndex + 2; i < data.length; i += 2 )
        {
          elements.push( componentLineTo.createObject( objOwner, { "x": data[ i ], "y": data[ i + 1 ] } ) )
        }

        if ( recordingInProgress ) { // construct a helper line / polygon
          if ( geometryType === 2 && elements.length > 2 )
          {
            newHelperLineElements = Array.from( elements ) // shallow copy
            let firstPoint = newHelperLineElements.pop()
            let centerPoint = crosshairPoint()
            newHelperLineElements.push( componentLineTo.createObject( helperPath, { "x": centerPoint.x, "y": centerPoint.y } ) )
            newHelperLineElements.push( firstPoint )
          }
          else
          {
            newHelperLineElements.push( componentMoveTo.createObject( helperPath, { "x": elements[ elements.length - 1 ].x, "y": elements[ elements.length - 1 ].y } ) )
            let centerPoint = crosshairPoint()
            newHelperLineElements.push( componentLineTo.createObject( helperPath, { "x": centerPoint.x, "y": centerPoint.y } ) )
          }
        }
      }
    }

    // reset shapes
    markerItems = markerItems.map( marker => marker.destroy() )
    if ( newLineElements.length === 0 )
      newLineElements.push( componentMoveTo.createObject( lineShapePath ) )
    if ( newPolygonElements.length === 0 )
      newPolygonElements.push( componentMoveTo.createObject( polygonShapePath ) )

    markerItems = newMarkerItems
    polygonShapePath.pathElements = newPolygonElements
    lineShapePath.pathElements = newLineElements
    lineOutlineShapePath.pathElements = newLineElements
    helperPath.pathElements = newHelperLineElements
  }

  onPositionChanged: {
    if ( !recordingInProgress )
      return

    updateHelperLine()
  }


  // keeps list of currently displayed marker items (an internal property)
  property var markerItems: []

  Component {
    id: componentMarker
    Item {
      property real posX: 0
      property real posY: 0
      property string markerType: highlight.markerType
      x: posX* highlight.mapTransformScale + highlight.mapTransformOffsetX* highlight.mapTransformScale - highlight.markerAnchorX
      y: posY*-highlight.mapTransformScale + highlight.mapTransformOffsetY*-highlight.mapTransformScale - highlight.markerAnchorY
      width: highlight.markerWidth
      height: highlight.markerHeight
      Rectangle {
          visible: markerType == "circle"
          anchors {
            centerIn: parent
            verticalCenterOffset: highlight.markerOffsetY
          }
          width: markerCircleSize
          height: markerCircleSize
          color: highlight.markerColor
          radius: width/2
      }
      Image {
          visible: markerType == "image"
          anchors.fill: parent
          source: highlight.markerImageSource
          sourceSize.width: width
          sourceSize.height: height
      }
    }
  }

  // item for rendering polygon/linestring geometries
  Shape {
    id: shape
    anchors.fill: parent

    transform: mapTransform

    Component {  id: componentLineTo; PathLine { } }
    Component {  id: componentMoveTo; PathMove { } }

    ShapePath {
        id: lineOutlineShapePath
        strokeWidth: highlight.lineWidth / highlight.mapTransformScale
        fillColor: "transparent"
        strokeColor: highlight.outlineColor
        capStyle: lineShapePath.capStyle
        joinStyle: lineShapePath.joinStyle
    }

    ShapePath {
      id: lineShapePath
      strokeColor: highlight.lineColor
      strokeWidth: (highlight.lineWidth - highlight.outlinePenWidth*2) / highlight.mapTransformScale  // negate scaling from the transform
      fillColor: "transparent"
      capStyle: ShapePath.RoundCap
      joinStyle: ShapePath.BevelJoin
    }

    ShapePath {
      id: polygonShapePath
      strokeColor: highlight.outlineColor
      strokeWidth: highlight.outlinePenWidth / highlight.mapTransformScale  // negate scaling from the transform
      fillColor: highlight.fillColor
      capStyle: ShapePath.FlatCap
      joinStyle: ShapePath.BevelJoin
    }

    ShapePath {
      id: helperPath
      fillColor: hasPolygon ? helperLineColor : "transparent"
      strokeColor: helperLineColor
      strokeWidth: (highlight.lineWidth - highlight.outlinePenWidth*2) / highlight.mapTransformScale  // negate scaling from the transform
      capStyle: ShapePath.RoundCap
      joinStyle: ShapePath.BevelJoin
    }
  }
}
