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
  property color markerColor: "yellow"
  property real markerWidth: 30 * QgsQuick.Utils.dp
  property real markerHeight: 30 * QgsQuick.Utils.dp
  property real markerAnchorX: markerWidth/2
  property real markerAnchorY: markerHeight/2
  property url markerImageSource   // e.g. "file:///home/martin/all_the_things.jpg"

  // feature+layer pair which determines what geometry is highlighted
  property var featureLayerPair: null

  // for transformation of the highlight to the correct location on the map
  property QgsQuick.MapSettings mapSettings

  //
  // internal properties not meant to be modified from outside
  //

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

  onFeatureLayerPairChanged: { // highlighting features
    let data = __inputUtils.extractGeometryCoordinates( featureLayerPair, mapSettings )

    let newMarkerItems = []
    let newLineElements = []
    let newPolygonElements = []

    let typeIdx = 0
    let pointDataStartIdx = 1
    let lpDataStartIdx = 2 // for lines and polygons

    console.log( "DATA: ", data )

    if ( data.length > lpDataStartIdx )
    {
      if ( data[ typeIdx ] === 0 ) // point (0)
      {
        newMarkerItems.push( componentMarker.createObject( highlight, { "posX": data[ pointDataStartIdx ], "posY": data[ pointDataStartIdx + 1 ] } ) )
      }
      else // line (1) or polygon (2)
      {
        if ( data.length < lpDataStartIdx + 3 ) // if this is the first point in line / polygon
        {
          // place temporary point marker
          newMarkerItems.push( componentMarker.createObject( highlight, { "posX": data[ lpDataStartIdx ], "posY": data[ lpDataStartIdx + 1 ] } ) )
        }
        else // iterate over points
        {
          let objOwner = ( data[ typeIdx ] === 1 ? lineShapePath : polygonShapePath )
          let elems = ( data[ typeIdx ] === 1 ? newLineElements : newPolygonElements )
          let dataLength = data[ 1 ] * 2

          // move brush to the first point
          elems.push( componentMoveTo.createObject( objOwner, {"x": data[ lpDataStartIdx ], "y": data[ lpDataStartIdx + 1 ] } ) )
          for ( let i = lpDataStartIdx + 2; i <= dataLength; i += 2 )
          {
            elems.push( componentLineTo.createObject( objOwner, { "x": data[ i ], "y": data[ i + 1 ] } ) )
          }
        }
        // TODO: place temporary line marker from last point to current crosshair
      }
    }

    for (var k = 0; k < markerItems.length; ++k)
      markerItems[k].destroy()
    markerItems = newMarkerItems

    if (newLineElements.length === 0)
      newLineElements.push(componentMoveTo.createObject(lineShapePath))
    lineShapePath.pathElements = newLineElements
    lineOutlineShapePath.pathElements = newLineElements

    if (newPolygonElements.length === 0)
      newPolygonElements.push(componentMoveTo.createObject(polygonShapePath))
    polygonShapePath.pathElements = newPolygonElements
  }

  // keeps list of currently displayed marker items (an internal property)
  property var markerItems: []

  Component {
    id: componentMarker
    Item {
      property real posX: 0
      property real posY: 0
      x: posX* highlight.mapTransformScale + highlight.mapTransformOffsetX* highlight.mapTransformScale - highlight.markerAnchorX
      y: posY*-highlight.mapTransformScale + highlight.mapTransformOffsetY*-highlight.mapTransformScale - highlight.markerAnchorY
      width: highlight.markerWidth
      height: highlight.markerHeight
      Rectangle {
          visible: highlight.markerType == "circle"
          anchors.fill: parent
          color: highlight.markerColor
          radius: width/2
      }
      Image {
          visible: highlight.markerType == "image"
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
  }

}
