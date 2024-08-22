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

  signal finishMeasurement()

  MM.MeasurementMapTool {
    id: mapTool
    mapSettings: root.map.mapSettings

    onCanCloseShape: function( canClose ) {
        measurePanel.canCloseShape = canClose;
    }

    onShapeArea: function( area ) {
        measurePanel.area = area;
    }

    onShapePerimeter: function( perimeter ) {
        measurePanel.perimeter = perimeter;
    }

    onCanUndo: function( canUndo ) {
        measurePanel.canUndo = canUndo;
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

  MMMeasureDrawer {
    id: measurePanel

    width: window.width
    mapCanvas: root.map

    onAddMeasurePoint: mapTool.addPoint( crosshair.recordPoint )
    onMeasureDone: finishMeasurementDialog.open()
    onMeasureFinished: root.finishMeasurement()
    onCloseShape: root.closeShape()
    onRepeat: root.repeatMeasure()
    onUndo: mapTool.removePoint()
  }

  MMCrosshair {
    id: crosshair
    anchors.fill: parent
    qgsProject: __activeProject.qgsProject
    mapSettings: root.map.mapSettings
    //hasLabel: true
    //crosshairLabelText: qsTr( "N/A")
    //crosshairLabelIcon: measurePanel.canCloseShape ? __style.closeShapeIcon : ""
  }

  MMMapLabel {
    id: mapLabel

    anchors.top: crosshair.bottom
    anchors.horizontalCenter: crosshair.horizontalCenter

    text: qsTr( "0.0 m" )
    bgColor: __style.forestColor
    textColor: __style.polarColor
    textBgColorInverted: false
    onClicked: console.log( "MapLabel" )
  }

  MMFinishMeasurementDialog {
    id: finishMeasurementDialog
    onFinishMeasurementRequested: root.finishMeasurement()
  }

  function onScreenPositionChanged() {
    let distance = mapTool.updateDistance( crosshair.recordPoint );

    measurePanel.length = distance.toFixed( 1 ) + " m";

    if ( measurePanel.canCloseShape ) {
      mapLabel.text = qsTr( "Close shape" )
      mapLabel.iconSource = __style.closeShapeIcon
    }
    else {
      mapLabel.text = measurePanel.length;
      mapLabel.iconSource = ""
    }
  }

  function closeShape()
  {
    guidelineController.allowed = false
    crosshair.visible = false
    measurePanel.closeShapeDone = true
    mapTool.closeShape()
  }

  function repeatMeasure()
  {
    guidelineController.allowed = true
    crosshair.visible = true
    measurePanel.closeShapeDone = false
    measurePanel.canCloseShape = false
    mapTool.repeat()
  }


  Component.onCompleted: map.mapSettings.extentChanged.connect( onScreenPositionChanged )
}
