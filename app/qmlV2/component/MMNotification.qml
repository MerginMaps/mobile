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
    radius: Style.notificationRadius
    color: {
      switch( type ) {
      case NotificationType.Information: return Style.informative
      case NotificationType.Success: return Style.positive
      case NotificationType.Warning: return Style.warning
      case NotificationType.Error: return Style.negative
      default: return Style.positive
      }
    }

    Rectangle {
      id: icon

      anchors.verticalCenter: parent.verticalCenter
      anchors.left: parent.left
      anchors.leftMargin: Style.commonSpacing
      width: 18 * __dp
      height: 18 * __dp
      color: text.color
      radius: width/2
    }

    Text {
      id: text

      anchors.verticalCenter: parent.verticalCenter
      anchors.left: icon.right
      width: parent.width - 3 * Style.commonSpacing - closeButton.width - icon.width
      text: message
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignLeft
      leftPadding: Style.commonSpacing - notification.innerSpacing
      font: Qt.font(Style.t3)
      clip: true
      color: {
        switch( type ) {
        case NotificationType.Information: return Style.deepOcean
        case NotificationType.Success: return Style.forest
        case NotificationType.Warning: return Style.earth
        case NotificationType.Error: return Style.grape
        }
      }
    }

    MMIcon {
      id: closeButton

      anchors.right: parent.right
      anchors.rightMargin: Style.commonSpacing
      anchors.verticalCenter: parent.verticalCenter
      scale: maRemove.containsMouse ? 1.2 : 1
      source: Style.closeIcon
      color: text.color

      MouseArea {
        id: maRemove

        anchors.fill: parent
        hoverEnabled: true
        onClicked: notificationModel.remove(id)
      }
    }
  }
}
