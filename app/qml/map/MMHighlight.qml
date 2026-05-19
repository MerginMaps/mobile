/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Shapes

import mm 1.0 as MM

import ".."

Item {
  id: highlight

  // geometry data array extracted from C++
  // geometry must be in map canvas CRS!
  property var geometryData

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
  readonly property url markerImageSource: __style.mapPinImage

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

    property real markerSize: highlight.markerSize === MMHighlight.MarkerSizes.Normal ? 18 * __dp : 21 * __dp
    property real lineWidth: highlight.lineWidth === MMHighlight.LineWidths.Normal ? 8 * __dp : 4 * __dp
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

  onGeometryDataChanged: constructHighlights()

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

    if ( !geometryData || geometryData.length === 0 )
    {
      // trigger repaint for empty geometries
      markerItems = markerItems.map( function (marker) { return marker.destroy() } )
      lineShapeItems = lineShapeItems.map( function (el) { return el.destroy() } )
      lineBorderShapeItems = lineBorderShapeItems.map( function (el) { return el.destroy() } )
      polygonShapeItems = polygonShapeItems.map( function (el) { return el.destroy() } )
      polygonRingBorderShapeItems = polygonRingBorderShapeItems.map( function (el) { return el.destroy() } )

      markerItems = [];
      lineShapeItems = []; lineBorderShapeItems = []; polygonShapeItems = []; polygonRingBorderShapeItems = [];
      
      lineShapePath.pathElements = [ componentMoveTo.createObject( lineShapePath ) ]
      lineBorderShapePath.pathElements = [ componentMoveTo.createObject( lineBorderShapePath ) ]
      polygonShapePath.pathElements = [ componentMoveTo.createObject( polygonShapePath ) ]
      polygonRingBorderPath.pathElements = [ componentMoveTo.createObject( polygonRingBorderPath ) ]

      return;
    }

    refTransformOffsetX = mapTransformOffsetX
    refTransformOffsetY = mapTransformOffsetY
    refTransformScale = mapTransformScale

    let data = geometryData 

    let newMarkerItems = []
    let newLineElements = []
    let newLineBorderElements = []
    let newPolygonElements = []
    let newPolygonRingBorderElements = []

    let geometryType = data[0]
    let dataStartIndex = ( geometryType === 0 ? 1 : 2 )

    if ( data.length > dataStartIndex )
    {
      if ( geometryType === 0 ) // point
      {
        let coords = []
        if ( data.length === 3 )
          coords.push( { x: data[dataStartIndex], y: data[dataStartIndex + 1] } )
        else
          for ( let it = dataStartIndex; it < data.length; it += 3 )
            coords.push( { x: data[it], y: data[it + 1] } )

        for ( let c = 0; c < coords.length; c++ )
        {
          if ( c < markerItems.length ) {
            markerItems[c].posX = coords[c].x
            markerItems[c].posY = coords[c].y
            newMarkerItems.push( markerItems[c] )
          } else {
            newMarkerItems.push( componentMarker.createObject( highlight, { "posX": coords[c].x, "posY": coords[c].y } ) )
          }
        }
        for ( let j = coords.length; j < markerItems.length; j++ ) markerItems[j].destroy()
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

        let linePartIdx = 0
        let polygonPartIdx = 0
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

          if ( geometryType === 1 )
          {
            let lineEl = linePartIdx < lineShapeItems.length ? lineShapeItems[ linePartIdx ] : componentPathPolyline.createObject( lineShapePath, { path: [] } )
            let borderEl = linePartIdx < lineBorderShapeItems.length ? lineBorderShapeItems[ linePartIdx ] : componentPathPolyline.createObject( lineBorderShapePath, { path: [] } )
            lineEl.path = newPath    
            borderEl.path = newPath
            newLineElements.push( lineEl )
            newLineBorderElements.push( borderEl )
            linePartIdx++
          }
          else
          {
            let polyEl = polygonPartIdx < polygonShapeItems.length ? polygonShapeItems[ polygonPartIdx ] : componentPathPolyline.createObject( polygonShapePath, { path: [] } )
            let ringEl = polygonPartIdx < polygonRingBorderShapeItems.length ? polygonRingBorderShapeItems[ polygonPartIdx ] : componentPathPolyline.createObject( polygonRingBorderPath, { path: [] } )
            polyEl.path = newPath
            ringEl.path = newPath
            newPolygonElements.push( polyEl )
            newPolygonRingBorderElements.push( ringEl )
            polygonPartIdx++
          }
        }

        for ( let j = newLineElements.length; j < lineShapeItems.length; j++ ) lineShapeItems[ j ].destroy()
        for ( let j = newLineBorderElements.length; j < lineBorderShapeItems.length; j++ ) lineBorderShapeItems[ j ].destroy()
        for ( let j = newPolygonElements.length; j < polygonShapeItems.length; j++ ) polygonShapeItems[ j ].destroy()
        for ( let j = newPolygonRingBorderElements.length; j < polygonRingBorderShapeItems.length; j++ ) polygonRingBorderShapeItems[ j ].destroy()
      }
    }

    if ( geometryType !== 0 ) markerItems.forEach( function (m) { if (m) m.destroy() } )
    if ( geometryType !== 1 ) { lineShapeItems.forEach( function (el) { if (el) el.destroy() } ); lineBorderShapeItems.forEach( function (el) { if (el) el.destroy() } ) }
    if ( geometryType !== 2 ) { polygonShapeItems.forEach( function (el) { if (el) el.destroy() } ); polygonRingBorderShapeItems.forEach( function (el) { if (el) el.destroy() } ) }

    markerItems = newMarkerItems
    
    lineShapeItems = newLineElements.slice()
    lineBorderShapeItems = newLineBorderElements.slice()
    polygonShapeItems = newPolygonElements.slice()
    polygonRingBorderShapeItems = newPolygonRingBorderElements.slice()

    if ( newLineElements.length === 0 ) {
      newLineElements.push( componentMoveTo.createObject( lineShapePath ) )
      newLineBorderElements.push( componentMoveTo.createObject( lineBorderShapePath ) )
    }
    if ( newPolygonElements.length === 0 ) {
      newPolygonElements.push( componentMoveTo.createObject( polygonShapePath ) )
      newPolygonRingBorderElements.push( componentMoveTo.createObject( polygonRingBorderPath ) )
    }

    lineShapePath.pathElements = newLineElements
    lineBorderShapePath.pathElements = newLineBorderElements
    polygonShapePath.pathElements = newPolygonElements
    polygonRingBorderPath.pathElements = newPolygonRingBorderElements

    shape.update()
  }

  property var markerItems: []
  property var lineShapeItems: []
  property var lineBorderShapeItems: []
  property var polygonShapeItems: []
  property var polygonRingBorderShapeItems: []

  Component {
    id: componentMarker

    Item {
      id: markerItem
      property real posX: 0
      property real posY: 0
      property int markerType: highlight.markerType

      x: ( posX * highlight.mapTransformScale + highlight.mapTransformOffsetX * highlight.mapTransformScale ) / highlight.displayDevicePixelRatio - ( highlight.markerWidth / 2 )
      y: ( posY * -highlight.mapTransformScale + highlight.mapTransformOffsetY * -highlight.mapTransformScale ) / highlight.displayDevicePixelRatio - highlight.markerHeight

      width: highlight.markerWidth
      height: highlight.markerHeight

      Rectangle {
        width: internal.markerSize
        height: width
        visible: markerItem.markerType === 0 

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
        visible: markerItem.markerType !== 0 
        source: highlight.markerImageSource
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
