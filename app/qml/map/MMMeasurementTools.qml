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
import "../gps"
import "../dialogs"

Item {
  id: root

  required property MMMapCanvas map
  required property MMPositionMarker positionMarkerComponent

  property var mapTool: mapTool

  signal finishMeasurement()

  Component.onCompleted: map.mapSettings.extentChanged.connect( onScreenPositionChanged )
  Component.onDestruction: map.mapSettings.extentChanged.disconnect( onScreenPositionChanged )

  MM.MeasurementMapTool {
    id: mapTool

    mapSettings: root.map.mapSettings
    activeLayer: __activeLayer.vectorLayer

    onCanCloseShape: function( canClose ) { console.log("A") }//measurePanel.canCloseShape = canClose; }
    onCanUndo: function( canUndo ) { console.log("A") } //measurePanel.canUndo = canUndo; }
    onShapeAreaAndPerimeter: function( area, perimeter ) {
      console.log("A")
      //measurePanel.area = __inputUtils.formatAreaInProjectUnit( area, 1 )
      //measurePanel.perimeter = __inputUtils.formatDistanceInProjectUnit( perimeter, 1 )
    }
  }

  MM.GuidelineController {
    id: guidelineController

    mapSettings: root.map.mapSettings
    crosshairPosition: crosshair.screenPoint
    realGeometry: __inputUtils.transformGeometryToMapWithLayer( mapTool.recordedGeometry, __activeLayer.vectorLayer, root.map.mapSettings )
  }

  MMHighlight {
    id: guideline

    height: root.map.height
    width: root.map.width

    markerColor: __style.deepOceanColor
    lineColor: __style.deepOceanColor
    lineStrokeStyle: ShapePath.DashLine
    lineWidth: MMHighlight.LineWidths.Narrow

    mapSettings: root.map.mapSettings
    geometry: guidelineController.guidelineGeometry
  }

  MMHighlight {
    id: highlight

    height: map.height
    width: map.width

    markerColor: __style.deepOceanColor
    lineColor: __style.deepOceanColor
    lineWidth: MMHighlight.LineWidths.Narrow

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.transformGeometryToMapWithLayer( mapTool.recordedGeometry, __activeLayer.vectorLayer, root.map.mapSettings )
  }

  // MMMeasureDrawer {
  //   id: measurePanel

  //   width: window.width
  //   mapCanvas: root.map

  //   //bind length and area to mapTool.length and mapTool.area / iconSource ===  or closeShape
  //   onAddMeasurePoint: mapTool.addPoint( crosshair.recordPoint )
  //   onMeasureDone: finishMeasurementDialog.open()
  //   onMeasureFinished: root.finishMeasurement()
  //   onCloseShape: root.closeShape()
  //   onRepeat: root.repeatMeasure()
  //   onUndo: mapTool.removePoint()
  // }

  MMCrosshair {
    id: crosshair

    anchors.fill: parent
    qgsProject: __activeProject.qgsProject
    mapSettings: root.map.mapSettings

  }

  // MMMapLabel {
  //   id: mapLabel

  //   text: qsTr( "0.0 m" )

  //   //implicitWidth: crosshair.width
  //   //implicitHeight: crosshair.crosshairForeground.height - 10
  //   bgColor: __style.forestColor
  //   textColor: __style.polarColor
  //   textBgColorInverted: false
  //   onClicked: console.log( "MapLabel: ", crosshair.height )

  //   y: crosshair.crosshairForeground.y + crosshair.crosshairForeground.height
  //   anchors.horizontalCenter: crosshair.crosshairForeground.horizontalCenter
  // }

  MMFinishMeasurementDialog {
    id: finishMeasurementDialog
    onFinishMeasurementRequested: root.finishMeasurement()
  }

  function addPoint()
  {
    mapTool.addPoint( crosshair.recordPoint )
  }

  function onScreenPositionChanged() {
    //mapTool.updateDistance( crosshair.recordPoint );
  }

  function closeShape()
  {
    guidelineController.allowed = false
    crosshair.visible = false
    //measurePanel.closeShapeDone = true
    mapTool.closeShape()
  }

  function repeatMeasure()
  {
    guidelineController.allowed = true
    crosshair.visible = true
    // measurePanel.closeShapeDone = false
    // measurePanel.canCloseShape = false
    mapTool.repeat()
  }
}
