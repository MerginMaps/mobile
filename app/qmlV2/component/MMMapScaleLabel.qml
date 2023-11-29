/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import Qt5Compat.GraphicalEffects
import "."
import ".."

// import lc 1.0

Item {
  id: control

  //  property alias mapSettings: scaleBarKit.mapSettings
  //  property alias preferredWidth: scaleBarKit.preferredWidth
  property string barText: "100 m" // scaleBarKit.distance + " " + scaleBarKit.units
  property real barWidth: 92 * __dp  // scaleBarKit.width
  property real lineWidth: 2.5 * __dp

  width: barWidth
  height: 46 * __dp

  property alias mapItem: blur.sourceItem

  //  ScaleBarKit { id: scaleBarKit }

  Rectangle {
    anchors.fill: fastBlur
    radius: height / 2
    color: styleV2.forestColor
  }

  FastBlur {
    id: fastBlur

    width: parent.width
    height: parent.height
    radius: 50
    opacity: 0.8
    source: ShaderEffectSource {
      id: blur
      sourceRect: Qt.rect(control.x, control.y, fastBlur.width, fastBlur.height)
    }
  }

  Column {
    width: parent.width
    anchors.centerIn: parent
    spacing: 3 * __dp

    Row {
      anchors.horizontalCenter: parent.horizontalCenter
      Rectangle {
        color: styleV2.forestColor
        width: control.lineWidth
        height: control.lineWidth * 2
        radius: height / 2
        y: (-height + control.lineWidth) / 2
      }
      Rectangle {
        color: styleV2.forestColor
        width: control.barWidth / 3 // scaleBarKit.width
        height: control.lineWidth
      }
      Rectangle {
        color: styleV2.forestColor
        width: control.lineWidth
        height: control.lineWidth * 2
        radius: height / 2
        y: (-height + control.lineWidth) / 2
      }
    }

    Text {
      font: styleV2.t3
      text: control.barText
      color: styleV2.forestColor
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      elide: Text.ElideRight
      width: parent.width
    }
  }

  NumberAnimation on opacity {
    id: fadeOut

    from: 0.7
    easing.type: Easing.OutExpo
    to: 0.0
    duration: 1000

    onStopped: {
      control.visible = false
      control.opacity = 1.0
    }
  }

  Timer {
    id: scaleBarTimer

    interval: 3000; running: false; repeat: false
    onTriggered: fadeOut.start()
  }

  onVisibleChanged: {
    if (control.visible) {
      fadeOut.stop()
      scaleBarTimer.restart()
    }
  }
}

