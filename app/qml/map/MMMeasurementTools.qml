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

  MMCrosshair {
    id: crosshair
    anchors.fill: parent
    qgsProject: __activeProject.qgsProject
    mapSettings: root.map.mapSettings
    hasLabel: true
    crosshairLabelText: "N/A"
  }

  MM.MeasurementMapTool {
    id: mapTool
    mapSettings: root.map.mapSettings

    onCanCloseShape: measurePanel.closeShapeActive = canClose
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
    onCloseShape: mapTool.closeShape()

  }

  MMFinishMeasurementDialog {
    id: finishMeasurementDialog
    onFinishMeasurementRequested: root.finishMeasurement()
  }

  function onScreenPositionChanged() {
    let distance = mapTool.updateDistance( crosshair.recordPoint );

    if (distance === 0.0 ) {
        measurePanel.length = "N/A";
    } else {
        measurePanel.length = distance.toFixed( 1 ) + " m";
    }

    crosshair.crosshairLabelText = measurePanel.length;
  }

  Component.onCompleted: map.mapSettings.extentChanged.connect( onScreenPositionChanged )
}
