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

Rectangle {
  id: root

  height: text.height + 2 * 15 * __dp
  anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined

  readonly property int innerSpacing: 5 * __dp

  radius: 12 * __dp
  color: {
    switch( type ) {
    case NotificationType.Information: return __style.informativeColor
    case NotificationType.Success: return __style.positiveColor
    case NotificationType.Warning: return __style.warningColor
    case NotificationType.Error: return __style.negativeColor
    default: return __style.positiveColor
    }
  }

  Component.onCompleted: scale = 0.0

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
      case NotificationType.None: return __style.checkmarkIcon
      case NotificationType.Waiting: return __style.waitingIcon
      case NotificationType.Check: return __style.checkmarkIcon
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
    leftPadding: 20 * __dp - root.innerSpacing
    font: __style.t3
    clip: true
    maximumLineCount: 3
    wrapMode: Text.WordWrap
    lineHeight: 1.4
    elide: Text.ElideRight
    color: {
      switch( type ) {
      case NotificationType.Information: return __style.deepOceanColor
      case NotificationType.Success: return __style.forestColor
      case NotificationType.Warning: return __style.earthColor
      case NotificationType.Error: return __style.grapeColor
      }
    }
  }

  MMIcon {
    id: closeButton

    anchors.right: parent.right
    anchors.rightMargin: 20 * __dp
    anchors.verticalCenter: parent.verticalCenter
    scale: maRemove.containsMouse ? 1.2 : 1
    source: __style.closeIcon
    color: text.color

    MouseArea {
      id: maRemove

      anchors.fill: parent
      hoverEnabled: true
      onClicked: __notificationModel.remove(id)
    }
  }

  Behavior on scale { NumberAnimation { easing.type: Easing.OutCubic; from: 0; to: 1.0; duration: 200 } }
}
