/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtLocation
import QtPositioning

import "../../app/qml/components"
import "../../app/qml/map"
import "../../app/qml/"

Page {
  id: pane

  Plugin {
    id: mapPlugin
    name: "osm"
  }

  Map {
    id: map

    anchors.fill: parent
    plugin: mapPlugin
    center: QtPositioning.coordinate(48.72, 21.25) // KE
    zoomLevel: 12
    property geoCoordinate startCentroid

    PinchHandler {
      id: pinch

      target: null
      onActiveChanged: if (active) {
                         map.startCentroid = map.toCoordinate(pinch.centroid.position, false)
                       }
      onScaleChanged: (delta) => {
                        map.zoomLevel += Math.log2(delta)
                        map.alignCoordinateToPoint(map.startCentroid, pinch.centroid.position)
                      }
      onRotationChanged: (delta) => {
                           map.bearing -= delta
                           map.alignCoordinateToPoint(map.startCentroid, pinch.centroid.position)
                         }
      grabPermissions: PointerHandler.TakeOverForbidden
    }

    WheelHandler {
      acceptedDevices: Qt.platform.pluginName === "cocoa" || Qt.platform.pluginName === "wayland"
                       ? PointerDevice.Mouse | PointerDevice.TouchPad
                       : PointerDevice.Mouse
      rotationScale: 1/120
      property: "zoomLevel"
    }

    DragHandler {
      target: null
      onTranslationChanged: (delta) => map.pan(-delta.x, -delta.y)
    }
  }

  MMMapScaleBar {
    anchors.horizontalCenter: parent.horizontalCenter
    y: 20
    sourceItem: map
    text: Math.floor(map.zoomLevel) + " zoom"
    barWidth: map.zoomLevel * 10
    onBarWidthChanged: visible = true
  }

  MMMapBlurLabel {
    x: 20
    y: 80
    width: parent.width - 40
    sourceItem: map
    text: "Mark the geometry on the map and click record"
  }

  Rectangle {
    anchors {
      bottom: parent.bottom
      left: parent.left
      right: parent.right
    }
    height: 30
    color: "white"

    Text {
      anchors.centerIn: parent
      text: map.center + "\tzoom: " + map.zoomLevel.toFixed(2)
    }
  }

  Column {
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.rightMargin: 20
    anchors.bottomMargin: 50
    spacing: 20

    MMMapButton {
      iconSource: __style.arrowLinkRightIcon
      onClicked: console.log("Map button clicked")
    }

    MMMapButton {
      iconSource: __style.searchIcon
      onClicked: console.log("Map button clicked")
    }
  }

  MMMapLabel {
    anchors.left: parent.left
    anchors.bottom: parent.bottom
    anchors.leftMargin: 20
    anchors.bottomMargin: 120

    text: "20.0 m"
    iconSource: __style.checkmarkIcon
  }

  MMMapLabel {
    anchors.left: parent.left
    anchors.bottom: parent.bottom
    anchors.leftMargin: 20
    anchors.bottomMargin: 50

    text: "20.0 m"
  }

  MMPositionMarker {
    xPos: 100
    yPos: 100
    direction: 30
    accuracyRingSize: 50

    Timer {
      interval: 2000; running: true; repeat: true
      onTriggered: parent.direction = Math.floor(Math.random() * 360)
    }

    Timer {
      interval: 5000; running: true; repeat: true
      onTriggered: parent.accuracyRingSize = 40 + Math.floor(Math.random() * 100)
    }
  }

  MMPositionMarker {
    xPos: 200
    yPos: 100
    direction: 330
    accuracyRingSize: 50
    trackingMode: true
  }
}
