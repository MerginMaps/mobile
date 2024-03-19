/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import mm 1.0 as MM

MMBlurBox {
  id: root

  property alias mapSettings: scaleBarKit.mapSettings
  property alias preferredWidth: scaleBarKit.preferredWidth

  timerInterval: 3000
  fadeOutDuration: 1000

  width: scaleBarKit.width + 48 * __dp
  height: 45 * __dp

  MM.ScaleBarKit {
    id: scaleBarKit
  }

  // Scale + text
  Column {
    width: scaleBarKit.width
    anchors.centerIn: parent

    Rectangle {
      width: parent.width
      height: 2 * __dp
      color: __style.forestColor

      Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: (width - height) / 2
        width: parent.height
        height: 6 * __dp
        color: parent.color
        radius: width / 2
      }

      Rectangle {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: (width - height) / 2
        width: parent.height
        height: 6 * __dp
        color: parent.color
        radius: width / 2
      }
    }

    Text {
      id: text

      height: 26 * __dp
      anchors.horizontalCenter: parent.horizontalCenter

      text: scaleBarKit.distance + " " + scaleBarKit.units
      color: __style.forestColor
      font: __style.t3
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
    }
  }

  function show() {
    root.visible = true
    root.restartAnimation()
  }
}
