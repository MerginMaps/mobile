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
import "../../app/qml/map/components"

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
    y: 120
    sourceItem: map
    preferredWidth: map.zoomLevel * 10
    visible: true
    onVisibleChanged: {
      if (!visible)
        visible = true
    }
  }

  MMMapBlurLabel {
    x: 20
    y: 80
    width: parent.width - 40
    sourceItem: map
    text: "Mark the geometry on the map and click record"
    visible: true
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
    anchors.bottom: toolbar.top
    anchors.rightMargin: 20
    anchors.bottomMargin: 50
    spacing: 20

    MMBusyIndicator {
      running: true

      Timer {
        interval: 4000; running: true; repeat: true
        onTriggered: parent.running = !parent.running
      }
    }

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
    id: gpsButton
    anchors.left: parent.left
    anchors.bottom: toolbar.top
    anchors.leftMargin: 20
    anchors.bottomMargin: 20
    text: "20.0 m"
    iconSource: __style.satelliteIcon
    onClicked: console.log("GPS button clicked")
  }

  MMMapLabel {
    id: positionTrackingButton

    anchors {
      left: parent.left
      leftMargin: 20
      bottom: gpsButton.top
      bottomMargin: 20
    }

    iconSource: __style.positionTrackingIcon
    text: "11:23:23"
    textBgColorInverted: true
  }

  MMMapButton {
    id: backButton

    anchors {
      left: parent.left
      top: parent.top
      leftMargin: 20
      topMargin: 20
    }

    iconSource: __style.backIcon
    onClicked: console.log("Map button clicked")
  }

  MMMapPicker {

    anchors {
      top: parent.top
      topMargin: 20
      left: backButton.right
      leftMargin: 20
    }

    width: Math.min( parent.width - backButton.width - 20 - 20 - 20, 500 )

    text: "Linestring layer ABCDEFGH"
    leftIconSource: __style.settingsIcon
  }

  MMToolbar {
    id: toolbar

    anchors.bottom: parent.bottom

    model: ObjectModel {
      MMToolbarButton { text: "Delete"; iconSource: __style.deleteIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "Edit Geometry"; iconSource: __style.editIcon; onClicked: console.log("tapped "+text) }
      MMToolbarButton { text: "Save"; iconSource: __style.doneCircleIcon; onClicked: console.log("tapped "+text) }
    }
  }
  
  MMPositionMarker {
    xPos: 300
    yPos: 200
    direction: 30
    accuracyRingSize: 60

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
    yPos: 300
    direction: 330
    accuracyRingSize: 30
    trackingMode: true
  }
}
