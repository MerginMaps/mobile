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

Row {
  id: notification

  width: listView.width * 0.95
  height: 25
  anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined

  Rectangle {
    width: parent.width
    height: parent.height
    color: "#BBFFFFFF" // Style.white
    border.color: Style.mediumGreen
    border.width: 1
    radius: 5
    Text {
      anchors.fill: parent
      anchors.leftMargin: 5
      text: message
      color: Style.forest
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignRight
      rightPadding: 20
    }
    Text {
      anchors.right: parent.right
      anchors.rightMargin: 5
      height: parent.height
      verticalAlignment: Text.AlignVCenter
      scale: maRemove.containsMouse ? 1.2 : 1
      text: "✘"
      color: maRemove.containsMouse ? Style.forest : Style.mediumGreen
      MouseArea {
        id: maRemove
        anchors.fill: parent
        hoverEnabled: true
        onClicked: _notificationModel.remove(id)
      }
    }
  }
}
