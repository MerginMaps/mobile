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
  id: navHighlight

  // color for line geometries
  property color lineColor: "black"
  // width for line geometries
  property real lineWidth: 2 * __dp

  // color for polygon geometries
  property color fillColor: "red"

  // width for outlines of lines and polygons
  property real outlinePenWidth: 1 * __dp
  // color for outlines of lines and polygons
  property color outlineColor: "black"

  // feature+layer pair which determines what geometry is highlighted
  property var destinationPair: null
  property var gpsPosition: __positionKit.positionCoordinate

  // for transformation of the highlight to the correct location on the map
  property QgsQuick.MapSettings mapSettings

  // properties used by markers (not able to use values directly from mapTransform
  // (no direct access to matrix no mapSettings' visible extent)
  property real mapTransformScale: 1
  property real mapTransformOffsetX: 0
  property real mapTransformOffsetY: 0
  property real displayDevicePixelRatio: 1

  // Reference view settings used for transformation of coordinates (needed for lines and polygons).
  // We convert their coordinates to screen coordinates of the current view, and then as user pans/zooms
  // the map, we use a transform to adjust the shape. The reason is that the transform uses single
  // precision floats, and if we used just map coordinates, we get numerical errors when the map gets
  // zoomed in. This approach is more stable (but does not avoid the issue 100%)
  property real refTransformScale: 1
  property real refTransformOffsetX: 0
  property real refTransformOffsetY: 0

  property real _srcX: 0
  property real _srcY: 0
  property real _dstX: 0
  property real _dstY: 0

  Connections {
      target: mapSettings
      onVisibleExtentChanged: {
          mapTransformScale = __inputUtils.mapSettingsScale(mapSettings)
          mapTransformOffsetX = __inputUtils.mapSettingsOffsetX(mapSettings)
          mapTransformOffsetY = __inputUtils.mapSettingsOffsetY(mapSettings)
          displayDevicePixelRatio = __inputUtils.mapSettingsDPR( mapSettings )
      }
  }

  function constructHighlights()
  {
    if ( !destinationPair || !gpsPosition || !mapSettings ) return

    refTransformOffsetX = mapTransformOffsetX
    refTransformOffsetY = mapTransformOffsetY
    refTransformScale = mapTransformScale

    let gpsMapCRS = __inputUtils.transformPoint( __inputUtils.coordinateReferenceSystemFromEpsgId( 4326 ), mapSettings.destinationCrs, mapSettings.transformContext(), __inputUtils.pointXY( gpsPosition.x, gpsPosition.y ) );
    let targetMapCRS = __inputUtils.transformPoint( destinationPair.layer.crs, mapSettings.destinationCrs, mapSettings.transformContext(), __inputUtils.extractPointFromFeature( destinationPair ) );

    _srcX =  ( gpsMapCRS.x + navHighlight.mapTransformOffsetX) * navHighlight.mapTransformScale / displayDevicePixelRatio
    _srcY = -( gpsMapCRS.y + navHighlight.mapTransformOffsetY) * navHighlight.mapTransformScale / displayDevicePixelRatio
    _dstX =  ( targetMapCRS.x + navHighlight.mapTransformOffsetX) * navHighlight.mapTransformScale / displayDevicePixelRatio
    _dstY = -( targetMapCRS.y + navHighlight.mapTransformOffsetY) * navHighlight.mapTransformScale / displayDevicePixelRatio
  }

  onDestinationPairChanged: constructHighlights()
  onGpsPositionChanged: constructHighlights()

  // enable anti-aliasing to make the higlight look nicer
  // https://stackoverflow.com/questions/48895449/how-do-i-enable-antialiasing-on-qml-shapes
  layer.enabled: true
  layer.samples: 4

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

    ShapePath {
      id: lineShapePath2
      strokeColor: "black"
      strokeWidth: (navHighlight.lineWidth - navHighlight.outlinePenWidth * 2 + 5 * __dp) / shapeTransform.scale // negate scaling from the transform
      fillColor: "black"
      capStyle: ShapePath.RoundCap
      joinStyle: ShapePath.BevelJoin

      startX: _srcX
      startY: _srcY

      PathLine { x: _dstX; y: _dstY; }
    }

    ShapePath {
      id: lineShapePath
      strokeColor: navHighlight.lineColor
      strokeWidth: (navHighlight.lineWidth - navHighlight.outlinePenWidth * 2) / shapeTransform.scale  // negate scaling from the transform
      fillColor: "transparent"
      capStyle: ShapePath.RoundCap
      joinStyle: ShapePath.BevelJoin

      startX: _srcX
      startY: _srcY

      PathLine { x: _dstX; y: _dstY; }
    }
  }
}
