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

Item {
    id: root

    required property MMMapCanvas map
    required property MMPositionMarker positionMarkerComponent

    property alias recordingMapTool: mapTool
    property var activeFeature

    signal canceled()
    signal addMeasurePoint()
    signal done(var featureLayerPair)

    MM.MeasurementMapTool {
        id: mapTool
    }

    MM.GuidelineController {
        id: guidelineController

        allowed: true //mapTool.state !== MM.RecordingMapTool.View && mapTool.recordingType !== MM.RecordingMapTool.StreamMode

        mapSettings: root.map.mapSettings
        insertPolicy: mapTool.insertPolicy
        crosshairPosition: crosshair.screenPoint
        realGeometry: __inputUtils.transformGeometryToMapWithLayer(mapTool.recordedGeometry, __activeLayer.vectorLayer, root.map.mapSettings)

        activeVertex: mapTool.activeVertex
        activePart: mapTool.activePart
        activeRing: mapTool.activeRing
    }

    MMCrosshair {
        id: crosshair

        anchors.fill: parent

        visible: true //mapTool.state !== MM.RecordingMapTool.View && mapTool.recordingType !== MM.RecordingMapTool.StreamMode

        qgsProject: __activeProject.qgsProject
        mapSettings: root.map.mapSettings
        shouldUseSnapping: !mapTool.isUsingPosition
        hasLabel: true
        crosshairLabelText: "58.4 m"
    }

    MMMeasureDrawer {
      id: measurePanel

      width: window.width

      mapCanvas: map

      onAddMeasurePoint: console.log(" Add measure ")
      onMeasureDone: finishMeasurementDialog.open()
    }


    function discardChanges() {
        mapTool.discardChanges()
        root.canceled()
    }

    function hasChanges() {
        return mapTool.hasChanges()
    }

    function onAddMeasurePoint() {
      console.log("Ponto de medição adicionado")
    }
}
