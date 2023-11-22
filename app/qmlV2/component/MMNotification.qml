/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import notificationType 1.0
import ".."

Rectangle {
  id: notification

  width: listView.width - 40 * __dp
  height: text.height + 2 * 15 * __dp
  anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined

  readonly property int innerSpacing: 5 * __dp

  radius: 12 * __dp
  color: {
    switch( type ) {
    case NotificationType.Information: return StyleV2.informativeColor
    case NotificationType.Success: return StyleV2.positiveColor
    case NotificationType.Warning: return StyleV2.warningColor
    case NotificationType.Error: return StyleV2.negativeColor
    default: return StyleV2.positiveColor
    }
  }

  MMIcon {
    id: leftIcon

    anchors.verticalCenter: parent.verticalCenter
    anchors.left: parent.left
    anchors.leftMargin: 20 * __dp
    width: 18 * __dp
    height: 18 * __dp
    color: text.color
    visible: icon !== NotificationType.None
    source: {
      switch( icon ) {
      case NotificationType.None: return StyleV2.checkmarkIcon
      case NotificationType.Waiting: return StyleV2.waitingIcon
      case NotificationType.Check: return StyleV2.checkmarkIcon
      }
    }
  }

  Text {
    id: text

    anchors.verticalCenter: parent.verticalCenter
    anchors.left: leftIcon.right
    width: parent.width - 60 * __dp - closeButton.width - leftIcon.width
    text: message
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignLeft
    leftPadding: 20 * __dp - notification.innerSpacing
    font: StyleV2.t3
    clip: true
    maximumLineCount: 3
    wrapMode: Text.WordWrap
    lineHeight: 1.4
    elide: Text.ElideRight
    color: {
      switch( type ) {
      case NotificationType.Information: return StyleV2.deepOceanColor
      case NotificationType.Success: return StyleV2.forestColor
      case NotificationType.Warning: return StyleV2.earthColor
      case NotificationType.Error: return StyleV2.grapeColor
      }
    }
  }

  MMIcon {
    id: closeButton

    anchors.right: parent.right
    anchors.rightMargin: 20 * __dp
    anchors.verticalCenter: parent.verticalCenter
    scale: maRemove.containsMouse ? 1.2 : 1
    source: StyleV2.closeIcon
    color: text.color

    MouseArea {
      id: maRemove

      anchors.fill: parent
      hoverEnabled: true
      onClicked: notificationModel.remove(id)
    }
  }
}
