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

import Input 0.1 as Input

import lc 1.0

import ".."

Item {
  id: highlight

  // geometry to highlight
  // geometry must be in map canvas CRS!
  property var geometry

  // for transformation of the highlight to the correct location on the map
  property Input.MapSettings mapSettings

  // point (marker) properties
  enum MarkerSizes { Normal, Bigger }
  enum MarkerTypes { Circle, Image }

  property int markerType: Highlight.MarkerTypes.Image
  property int markerSize: Highlight.MarkerSizes.Normal

  property color markerColor: __style.grapeColor
  property color markerBorderColor: __style.whiteColor

  property real markerWidth: 40 * __dp // based on marker image size
  property real markerHeight: 53 * __dp // based on marker image size
  property real markerBorderWidth: 2 * __dp

  // line properties
  enum LineWidths { Normal, Narrow }

  property int lineWidth: Highlight.LineWidths.Normal
  property real lineBorderWidth: 4 * __dp // on top of width

  property color lineColor: __style.grapeColor
  property color lineBorderColor: __style.whiteColor

  property int lineStrokeStyle: ShapePath.SolidLine // (solid line / dashed line)

  // polygon properties
  property color polygonFillColor: __style.grapeTransparentColor
  property color polygonRingColor: __style.grapeColor
  property color polygonRingBorderColor: __style.whiteColor

  property real polygonRingWidth: 8 * __dp
  property real polygonRingBorderWidth: 0 // on top of ring width

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

  QtObject {
    id: internal

    property real markerSize: highlight.markerSize === Highlight.MarkerSizes.Normal ? 18 * __dp : 21 * __dp
    property real lineWidth: highlight.lineWidth === Highlight.LineWidths.Normal ? 8 * __dp : 4 * __dp
  }

  Connections {
      target: mapSettings
      function onVisibleExtentChanged() {
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
    if ( !mapSettings ) return

    if ( !geometry )
    {
      // trigger repaint for empty geometries
      markerItems = markerItems.map( function (marker) { return marker.destroy() } )

      let newLineElements = [];
      newLineElements.push( componentMoveTo.createObject( lineShapePath ) )

      let newPolygonElements = [];
      newPolygonElements.push( componentMoveTo.createObject( polygonShapePath ) )

      markerItems = [];
      lineShapePath.pathElements = newLineElements
      polygonShapePath.pathElements = newPolygonElements
      lineBorderShapePath.pathElements = newLineElements
      polygonRingBorderPath.pathElements = newPolygonElements

      return;
    }

    refTransformOffsetX = mapTransformOffsetX
    refTransformOffsetY = mapTransformOffsetY
    refTransformScale = mapTransformScale

    let data = __inputUtils.extractGeometryCoordinates( highlight.geometry )

    let newMarkerItems = []
    let newLineElements = []
    let newPolygonElements = []

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
                                                              "markerType": Highlight.MarkerTypes.Circle
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

          // skip this part/ring if it is empty
          if ( pointsCount === 0 )
          {
            continue
          }

          let newPath = []

          // Draw lines for points in the segment
          for ( k = i; k < i + pointsCount * 2; k += 2 )
          {
            let x1 = transformX( data[k] )
            let y1 = transformY( data[k+1] )
            newPath.push( Qt.point( x1, y1 ) )
          }
          i = k

          elements.push( componentPathPolyline.createObject( objOwner, { path: newPath } ) )
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
    polygonRingBorderPath.pathElements = newPolygonElements
    lineShapePath.pathElements = newLineElements
    lineBorderShapePath.pathElements = newLineElements
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

      property int markerType: highlight.markerType

      x: ( posX *  highlight.mapTransformScale + highlight.mapTransformOffsetX *  highlight.mapTransformScale ) / displayDevicePixelRatio - ( highlight.markerWidth / 2 )
      y: ( posY * -highlight.mapTransformScale + highlight.mapTransformOffsetY * -highlight.mapTransformScale ) / displayDevicePixelRatio - highlight.markerHeight

      width: highlight.markerWidth
      height: highlight.markerHeight

      Rectangle {

        width: internal.markerSize
        height: width

        visible: markerType === Highlight.MarkerTypes.Circle

        anchors {
          bottom: parent.bottom
          bottomMargin: -width / 2
          horizontalCenter: parent.horizontalCenter
        }

        radius: width / 2
        color: highlight.markerColor
        border.color: highlight.markerBorderColor
        border.width: highlight.markerBorderWidth
      }

      Image {
        visible: markerType === Highlight.MarkerTypes.Image

        source: __style.mapPinIcon
        sourceSize.width: parent.width
        sourceSize.height: parent.height
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

    Component {  id: componentMoveTo; PathMove { } }
    Component {  id: componentPathPolyline; PathPolyline { } }

    ShapePath {
        id: lineBorderShapePath
        strokeWidth: ( highlight.lineBorderWidth + internal.lineWidth ) / shapeTransform.scale  // negate scaling from the transform
        fillColor: "transparent"
        strokeColor: highlight.lineBorderColor
        capStyle: lineShapePath.capStyle
        joinStyle: lineShapePath.joinStyle
    }

    ShapePath {
      id: lineShapePath
      strokeColor: highlight.lineColor
      strokeStyle: highlight.lineStrokeStyle
      strokeWidth: internal.lineWidth / shapeTransform.scale  // negate scaling from the transform
      fillColor: "transparent"
      capStyle: ShapePath.RoundCap
      joinStyle: ShapePath.BevelJoin
    }

    ShapePath {
      // Used as a border to polygon ring - we do not use it currently.
      // If the simple path for rings is enough (without borders),
      // we can safely remove this one.
      id: polygonRingBorderPath
      strokeColor: highlight.polygonRingBorderColor
      strokeStyle: ShapePath.SolidLine
      strokeWidth: (highlight.polygonRingBorderWidth + highlight.polygonRingWidth) / shapeTransform.scale  // negate scaling from the transform
      fillColor: "transparent"
      capStyle: ShapePath.RoundCap
      joinStyle: ShapePath.BevelJoin
    }

    ShapePath { // rings + fill
      id: polygonShapePath
      strokeColor: highlight.polygonRingColor
      strokeWidth: highlight.polygonRingWidth / shapeTransform.scale  // negate scaling from the transform
      fillColor: highlight.polygonFillColor
      capStyle: ShapePath.FlatCap
      joinStyle: ShapePath.BevelJoin
    }
  }
}
