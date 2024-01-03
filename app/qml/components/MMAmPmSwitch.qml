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

// Prepared switch, but to used yet

Switch {
  id: control

  indicator: Rectangle {
    implicitWidth: 120 * __dp
    implicitHeight: 56 * __dp
    radius: implicitHeight / 2
    color: __style.lightGreenColor

    Rectangle {
      x: control.checked ? parent.width - width - 5 * __dp : 5 * __dp
      width: 55 * __dp
      height: 40 * __dp
      radius: width / 2
      color: control.enabled ? __style.grassColor : __style.mediumGreenColor
      anchors.verticalCenter: parent.verticalCenter
    }

    Text {
      width: 65 * __dp
      height: parent.height
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      font: __style.t4
      color: __style.forestColor
      text: qsTr("AM")
    }

    Text {
      width: 65 * __dp
      height: parent.height
      anchors.right: parent.right
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      font: __style.t4
      color: __style.forestColor
      text: qsTr("PM")
    }
  }
}
