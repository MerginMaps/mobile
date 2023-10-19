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

import "../../app/qmlV2/component"
import "../../app/qmlV2/Style.js" as Style

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
    }

    MMMapButton {
    }
  }

  MMMapLabel {
    anchors.left: parent.left
    anchors.bottom: parent.bottom
    anchors.leftMargin: 20
    anchors.bottomMargin: 50

    text: "20.0 m"
  }
}
