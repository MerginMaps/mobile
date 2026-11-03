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
import QtQml

import mm 1.0 as MM
import MMInput

import ".."

Item {
  id: highlight

  // geometry to highlight
  // geometry must be in map canvas CRS!
  property qgsGeometry geometry

  // for transformation of the highlight to the correct location on the map
  property MM.MapSettings mapSettings

  // point (marker) properties
  enum MarkerSizes { Normal, Bigger }
  enum MarkerTypes { Circle, Image }

  property int markerType: MMHighlight.MarkerTypes.Image
  property int markerSize: MMHighlight.MarkerSizes.Normal

  property color markerColor: __style.grapeColor
  property color markerBorderColor: __style.polarColor

  property real markerWidth: 40 * __dp // based on marker image size
  property real markerHeight: 53 * __dp // based on marker image size
  property real markerBorderWidth: 2 * __dp

  // line properties
  enum LineWidths { Normal, Narrow }

  property int lineWidth: MMHighlight.LineWidths.Normal
  property real lineBorderWidth: 4 * __dp // on top of width

  property color lineColor: __style.grapeColor
  property color lineBorderColor: __style.polarColor

  property int lineStrokeStyle: ShapePath.SolidLine // (solid line / dashed line)

  // polygon properties
  property color polygonFillColor: __style.grapeTransparentColor
  property color polygonRingColor: __style.grapeColor
  property color polygonRingBorderColor: __style.polarColor

  property real polygonRingWidth: 8 * __dp
  property real polygonRingBorderWidth: 0 // on top of ring width

  // Affine transform from map coordinates to screen coordinates (including map rotation) as [a, b, c, d, e, f]:
  //   x_screen = a * x_map + b * y_map + e
  //   y_screen = c * x_map + d * y_map + f
  property var mapTransform: __inputUtils.mapSettingsTransform( mapSettings )

  // Reference view settings used for transformation of coordinates (needed for lines and polygons).
  // We convert their coordinates to screen coordinates of the current view, and then as user pans/zooms
  // the map, we use a transform to adjust the shape. The reason is that the transform uses single
  // precision floats, and if we used just map coordinates, we get numerical errors when the map gets
  // zoomed in. This approach is more stable (but does not avoid the issue 100%)
  property var refTransform: [1, 0, 0, 0, 1, 0]

  QtObject {
    id: internal

    property real markerSize: highlight.markerSize === MMHighlight.MarkerSizes.Normal ? 18 * __dp : 21 * __dp
    property real lineWidth: highlight.lineWidth === MMHighlight.LineWidths.Normal ? 8 * __dp : 4 * __dp
  }

  Connections {
      target: mapSettings
      function onVisibleExtentChanged() {
          mapTransform = __inputUtils.mapSettingsTransform( mapSettings )
      }
  }

  onGeometryChanged: constructHighlights()

  // Transforms point from map CRS to screen XY with regards to scale, rotation and HighDPI
  function transformPoint( transform, xcoord, ycoord )
  {
    return Qt.point( transform[0] * xcoord + transform[1] * ycoord + transform[4],
                     transform[2] * xcoord + transform[3] * ycoord + transform[5] )
  }

  function constructHighlights()
  {
    if ( !mapSettings ) return

    if ( __inputUtils.isEmptyGeometry( geometry ) )
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

    refTransform = mapTransform

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
                                                              "markerType": MMHighlight.MarkerTypes.Circle
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
            newPath.push( transformPoint( refTransform, data[k], data[k+1] ) )
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

      // marker stays upright when the map is rotated, only its position changes
      property point screenPoint: highlight.transformPoint( highlight.mapTransform, posX, posY )

      x: screenPoint.x - ( highlight.markerWidth / 2 )
      y: screenPoint.y - highlight.markerHeight

      width: highlight.markerWidth
      height: highlight.markerHeight

      Rectangle {

        width: internal.markerSize
        height: width

        visible: markerType === MMHighlight.MarkerTypes.Circle

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
        visible: markerType === MMHighlight.MarkerTypes.Image

        source: __style.mapPinImage
        sourceSize.width: parent.width
        sourceSize.height: parent.height
      }
    }
  }

  // item for rendering polygon/linestring geometries
  Shape {
    id: shape
    anchors.fill: parent

    // The newer CurveRenderer is used for dashed lines to avoid a bug causing off-screen lines to be generated as empty
    // see https://github.com/MerginMaps/mobile/issues/2280
    preferredRendererType: highlight.lineStrokeStyle === ShapePath.DashLine ? Shape.CurveRenderer : Shape.UnknownRenderer

    transform: Matrix4x4 {
        // this matrix is just doing transform from old view settings (scale, rotation, offset) for which we have
        // calculated coordinates in constructHighlight to new view settings (that are active now):
        //   relative = current * inverse( reference ), where both are map to screen affine transforms
        id: shapeTransform

        property real refDet: refTransform[0] * refTransform[3] - refTransform[1] * refTransform[2]
        property real curDet: mapTransform[0] * mapTransform[3] - mapTransform[1] * mapTransform[2]

        // uniform scale change, used to negate the scaling of the stroke width
        property real scale: Math.sqrt( Math.abs( curDet / refDet ) )

        // inverse of the reference transform
        property real i11: refTransform[3] / refDet
        property real i12: -refTransform[1] / refDet
        property real i21: -refTransform[2] / refDet
        property real i22: refTransform[0] / refDet
        property real i13: -( i11 * refTransform[4] + i12 * refTransform[5] )
        property real i23: -( i21 * refTransform[4] + i22 * refTransform[5] )

        property real m11: mapTransform[0] * i11 + mapTransform[1] * i21
        property real m12: mapTransform[0] * i12 + mapTransform[1] * i22
        property real m13: mapTransform[0] * i13 + mapTransform[1] * i23 + mapTransform[4]
        property real m21: mapTransform[2] * i11 + mapTransform[3] * i21
        property real m22: mapTransform[2] * i12 + mapTransform[3] * i22
        property real m23: mapTransform[2] * i13 + mapTransform[3] * i23 + mapTransform[5]

        matrix: Qt.matrix4x4( m11, m12, 0, m13,
                              m21, m22, 0, m23,
                              0,   0,   1, 0,
                              0,   0,   0, 1)
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
