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
import notificationType 1.0

Row {
  id: notification

  width: listView.width - 2 * Style.commonSpacing
  height: Style.notificationHeight
  anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined

  readonly property int innerSpacing: 5 * __dp

  Rectangle {
    width: parent.width
    height: parent.height
    color: Style.forest
    radius: Style.notificationRadius

    Rectangle {
      id: borderRect

      anchors.centerIn: parent
      width: parent.width - notification.innerSpacing
      height: parent.height - notification.innerSpacing
      radius: Style.notificationRadius
      color: Style.transparent
      border.width: __dp
      border.color: {
        switch( type ) {
        case NotificationType.Information: return Style.sky
        case NotificationType.Warning: return Style.warning
        case NotificationType.Error: return Style.negative
        default: return Style.positive
        }
      }
    }

    Rectangle {
      id: icon

      anchors.verticalCenter: parent.verticalCenter
      anchors.left: parent.left
      anchors.leftMargin: Style.commonSpacing
      width: 18 * __dp
      height: 18 * __dp
      color: borderRect.border.color
      radius: width/2
    }

    Text {
      anchors.verticalCenter: parent.verticalCenter
      anchors.left: icon.right
      width: parent.width - 3 * Style.commonSpacing - closeButton.width - icon.width
      text: message
      color: Style.white
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignLeft
      leftPadding: Style.commonSpacing - notification.innerSpacing
      font: Qt.font(Style.t3)
      clip: true
    }

    Image {
      id: closeButton

      anchors.right: parent.right
      anchors.rightMargin: Style.commonSpacing
      anchors.verticalCenter: parent.verticalCenter
      scale: maRemove.containsMouse ? 1.2 : 1
      source: Style.closeIcon

      MouseArea {
        id: maRemove

        anchors.fill: parent
        hoverEnabled: true
        onClicked: notificationModel.remove(id)
      }
    }
  }
}
