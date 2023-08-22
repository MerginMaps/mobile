/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import "../Style.js" as Style

Item {
  id: control

  z: 100
  anchors.top: parent.top
  width: parent.width
  height: parent.height

  Rectangle {
    anchors.bottom: parent.bottom
    width: control.width
    height: 20
    color: Style.white

    Text {
      text: listView.count
      anchors.centerIn: parent
      color: Style.forest
    }
  }

  ListView {
    id: listView

    anchors.top: parent.top
    width: parent.width
    height: 25 * listView.count + spacing * (listView.count - 1)
    spacing: 3
    clip: true
    model: _notificationModel
    delegate: MMNotification {

    }

    add: Transition {
      NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 200 }
      NumberAnimation { property: "scale"; easing.type: Easing.OutCubic; from: 0; to: 1.0; duration: 200 }
    }

  }
}
