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

import QgsQuick 0.1 as QgsQuick

import lc 1.0

import ".."

Item {
  id: highlight

  // geometry to highlight
  // geometry must be in map canvas CRS!
  property var geometry

  // color for line geometries
  property color lineColor: InputStyle.highlightLineColor
  // width for line geometries
  property real lineWidth: InputStyle.highlightLineWidth
  // stroke style for line geometries (solid line / dashed line)
  property int lineStrokeStyle: ShapePath.SolidLine

  // color for polygon geometries
  property color fillColor: InputStyle.highlightFillColor

  // width for outlines of lines and polygons
  property real outlinePenWidth: InputStyle.highlightOutlinePenWidth
  // color for outlines of lines and polygons
  property color outlineColor: InputStyle.highlightOutlineColor

  property string markerType: "image"   // "circle" or "image"
  property color markerColor: "grey"
  property color markerOutlineColor: "black"
  property real markerOutlineWidth: 1 * __dp < 1 ? 1 : 1 * __dp
  property real markerWidth: InputStyle.mapMarkerWidth
  property real markerHeight: InputStyle.mapMarkerHeight
  property real markerAnchorX: markerWidth / 2
  property real markerAnchorY: InputStyle.mapMarkerAnchorY
  property url markerImageSource: InputStyle.mapMarkerIcon
  property url markerCircleIconSource: InputStyle.noIcon

  // for transformation of the highlight to the correct location on the map
  property QgsQuick.MapSettings mapSettings

  //
  // internal properties not meant to be modified from outside
  //
  property real markerOffsetY: 14 * __dp // for circle marker type to be aligned with crosshair
  property real markerCircleSize: 15 * __dp

  // properties used by markers (not able to use values directly from mapTransform
  // (no direct access to matrix no mapSettings' visible extent)
  property real mapTransformScale: __inputUtils.mapSettingsScale(mapSettings)
  property real mapTransformOffsetX: __inputUtils.mapSettingsOffsetX(mapSettings)
  property real mapTransformOffsetY: __inputUtils.mapSettingsOffsetY(mapSettings)
  property real displayDevicePixelRatio: __inputUtils.mapSettingsDPR( mapSettings )

  // Reference view settings used for transformation of coordinates (needed for lines and polygons).
  // We convert their coordinates to screen coordinates of the current view, and then as user pans/zooms
  // the map, we use a transform to adjust the shape. The reason is that the transform uses single
  // precision floats, and if we used just map coordinates, we get numerical errors when the map gets
  // zoomed in. This approach is more stable (but does not avoid the issue 100%)
  property real refTransformScale: 1
  property real refTransformOffsetX: 0
  property real refTransformOffsetY: 0

  Connections {
      target: mapSettings
      onVisibleExtentChanged: {
          mapTransformScale = __inputUtils.mapSettingsScale(mapSettings)
          mapTransformOffsetX = __inputUtils.mapSettingsOffsetX(mapSettings)
          mapTransformOffsetY = __inputUtils.mapSettingsOffsetY(mapSettings)
          displayDevicePixelRatio = __inputUtils.mapSettingsDPR( mapSettings )
      }
  }

  onGeometryChanged: constructHighlights()

  // Transforms X coordinate from map CRS to screen XY with regards to scale and HighDPI
  function transformX( xcoord )
  {
    return ( xcoord + highlight.mapTransformOffsetX ) * highlight.mapTransformScale / displayDevicePixelRatio
  }

  // Transforms Y coordinate from map CRS to screen XY with regards to scale and HighDPI
  function transformY( ycoord )
  {
    return -( ycoord + highlight.mapTransformOffsetY ) * highlight.mapTransformScale / displayDevicePixelRatio
  }

  function constructHighlights()
  {
    if ( !geometry || !mapSettings ) return

    refTransformOffsetX = mapTransformOffsetX
    refTransformOffsetY = mapTransformOffsetY
    refTransformScale = mapTransformScale

    let data = __inputUtils.extractGeometryCoordinates( highlight.geometry )

    let newMarkerItems = []
    let newLineElements = []
    let newPolygonElements = []
    let newGuideLineElements = []

    let geometryType = data[0] // type of geometry - 0: point, 1: linestring, 2: polygon
    let dataStartIndex = ( geometryType === 0 ? 1 : 2 ) // point data starts from index 1, others from index 2

    if ( data.length > dataStartIndex )
    {
      if ( geometryType === 0 ) // point
      {
        if ( data.length === 3 )
        {
          newMarkerItems.push( componentMarker.createObject( highlight, { "posX": data[dataStartIndex], "posY": data[dataStartIndex + 1] } ) )
        }
        else
        {
          let it = 0;
          // this is multipoint [0, x1, y1, 0, x2, y2, 0, x3, y3, 0,..]
          for ( it = dataStartIndex; it < data.length; it += 3 )
          {
            newMarkerItems.push( componentMarker.createObject( highlight, {
                                                                "posX": data[it],
                                                                "posY": data[it + 1]
                                                              } ) )
          }
        }
      }
      else // line or polygon
      {
        // place temporary point marker if this is the first point in line / polygon
        if ( data.length < dataStartIndex + 3 )
        {
          newMarkerItems.push( componentMarker.createObject( highlight, {
                                                              "posX": data[ dataStartIndex ],
                                                              "posY": data[ dataStartIndex + 1 ],
                                                              "markerType": "circle"
                                                            } ) )
        }

        let objOwner = ( geometryType === 1 ? lineShapePath : polygonShapePath )
        let elements = ( geometryType === 1 ? newLineElements : newPolygonElements )

        // Create (multi) geometry for the highlight
        let i = 0
        let k = 0
        while ( i < data.length )
        {
          let geomType = data[ i++ ];
          let pointsCount = data[ i++ ];
          // Move to the first point
          let x0 = transformX( data[i] )
          let y0 = transformY( data[i+1] )
          elements.push( componentMoveTo.createObject( objOwner, { "x": x0, "y": y0 } ) )
          // Draw lines for rest of points in the segment
          for ( k = i + 2; k < i + pointsCount * 2; k += 2 )
          {
            let x1 = transformX( data[k] )
            let y1 = transformY( data[k+1] )
            elements.push( componentLineTo.createObject( objOwner, { "x": x1, "y": y1 } ) )
          }
          i = k
        }
      }
    }

    // trigger repaint for empty geometries
    markerItems = markerItems.map( function (marker) { return marker.destroy() } )
    if ( newLineElements.length === 0 )
      newLineElements.push( componentMoveTo.createObject( lineShapePath ) )
    if ( newPolygonElements.length === 0 )
      newPolygonElements.push( componentMoveTo.createObject( polygonShapePath ) )

    markerItems = newMarkerItems
    polygonShapePath.pathElements = newPolygonElements
    lineShapePath.pathElements = newLineElements
    lineOutlineShapePath.pathElements = newLineElements
  }

  // keeps list of currently displayed marker items (an internal property)
  property var markerItems: []

  // enable anti-aliasing to make the higlight look nicer
  // https://stackoverflow.com/questions/48895449/how-do-i-enable-antialiasing-on-qml-shapes
  layer.enabled: true
  layer.samples: 4

  Component {
    id: componentMarker

    Item {

      property real posX: 0
      property real posY: 0
      property string markerType: highlight.markerType

      x: ( posX *  highlight.mapTransformScale + highlight.mapTransformOffsetX *  highlight.mapTransformScale ) / displayDevicePixelRatio - highlight.markerAnchorX
      y: ( posY * -highlight.mapTransformScale + highlight.mapTransformOffsetY * -highlight.mapTransformScale ) / displayDevicePixelRatio - highlight.markerAnchorY

      width: highlight.markerWidth
      height: highlight.markerHeight

      Rectangle {
          visible: markerType === "circle"
          anchors {
            centerIn: parent
            verticalCenterOffset: highlight.markerOffsetY
          }
          width: markerCircleSize
          height: markerCircleSize
          color: highlight.markerColor
          radius: width/2
      }

      Rectangle {
        visible: markerType === "circleWithIcon"

        anchors {
          centerIn: parent
          verticalCenterOffset: highlight.markerOffsetY
        }

        width: markerCircleSize
        height: markerCircleSize

        color: highlight.markerColor

        radius: width / 2

        border.color: highlight.markerOutlineColor
        border.width: highlight.markerOutlineWidth

        Image {
          anchors.centerIn: parent

          source: highlight.markerCircleIconSource

          width: parent.width - parent.width / 5
          height: parent.height - parent.height / 5
          sourceSize.width: width
          sourceSize.height: height
        }
      }

      Image {
          visible: markerType === "image"
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

    transform: Matrix4x4 {
        // the formula for x coordinate for map to screen coordinates conversion goes like this:
        //   x_screen = (x_map + offset_x) * scale / ddp
        // this matrix is just doing transform from old view settings (scale, offset_x, offset_y) for which we have
        // calculated coordinates in constructHighlight to new view settings (that are active now).
        id: shapeTransform
        property real scale: mapTransformScale / refTransformScale
        property real offsetX:  (mapTransformOffsetX - refTransformOffsetX) * mapTransformScale / displayDevicePixelRatio
        property real offsetY: -(mapTransformOffsetY - refTransformOffsetY) * mapTransformScale / displayDevicePixelRatio

        matrix: Qt.matrix4x4( scale, 0,     0, offsetX,
                              0,     scale, 0, offsetY,
                              0,     0,     1, 0,
                              0,     0,     0, 1)
    }

    Component {  id: componentLineTo; PathLine { } }
    Component {  id: componentMoveTo; PathMove { } }

    ShapePath {
        id: lineOutlineShapePath
        strokeWidth: highlight.lineWidth / shapeTransform.scale  // negate scaling from the transform
        fillColor: "transparent"
        strokeColor: highlight.outlineColor
        capStyle: lineShapePath.capStyle
        joinStyle: lineShapePath.joinStyle
    }

    ShapePath {
      id: lineShapePath
      strokeColor: highlight.lineColor
      strokeStyle: highlight.lineStrokeStyle
      strokeWidth: (highlight.lineWidth - highlight.outlinePenWidth*2) / shapeTransform.scale  // negate scaling from the transform
      fillColor: "transparent"
      capStyle: ShapePath.RoundCap
      joinStyle: ShapePath.BevelJoin
    }

    ShapePath {
      id: polygonShapePath
      strokeColor: highlight.outlineColor
      strokeWidth: highlight.outlinePenWidth / shapeTransform.scale  // negate scaling from the transform
      fillColor: highlight.fillColor
      capStyle: ShapePath.FlatCap
      joinStyle: ShapePath.BevelJoin
    }
  }
}
