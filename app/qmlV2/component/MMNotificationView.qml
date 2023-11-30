/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

Item {
  id: control

  anchors.top: parent.top
  anchors.topMargin: 20 * __dp
  width: parent.width
  height: parent.height

  // just for information - will be removed in release version
  Rectangle {
    anchors.bottom: parent.bottom
    width: control.width
    height: 20
    color: __style.whiteColor

    Text {
      text: listView.count
      anchors.centerIn: parent
      color: __style.forestColor
    }
  }

  ListView {
    id: listView

    anchors.top: parent.top
    width: parent.width
    height: contentHeight
    spacing: 3 * __dp
    clip: true
    model: notificationModel
    delegate: MMNotification {

    }

    add: Transition {
      NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 200 }
      NumberAnimation { property: "scale"; easing.type: Easing.OutCubic; from: 0; to: 1.0; duration: 200 }
    }

  }
}
