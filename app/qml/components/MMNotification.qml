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

Rectangle {
  id: root

  height: text.height + 2 * 15 * __dp
  anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined

  readonly property int innerSpacing: 5 * __dp

  radius: 12 * __dp
  color: {
    switch( model.type ) {
    case MM.NotificationType.Information: return __style.informativeColor
    case MM.NotificationType.Success: return __style.positiveColor
    case MM.NotificationType.Warning: return __style.warningColor
    case MM.NotificationType.Error: return __style.negativeColor
    default: return __style.positiveColor
    }
  }

  Component.onCompleted: scale = 0.0

  MMIcon {
    id: leftIcon

    anchors.verticalCenter: parent.verticalCenter
    anchors.left: parent.left
    anchors.leftMargin: 20 * __dp
    size: model.icon === MM.NotificationType.Check ? __style.icon18 : __style.icon24
    color: text.color
    visible: model.icon !== MM.NotificationType.None
    source: {
      switch( model.icon ) {
      case MM.NotificationType.None: return ""
      case MM.NotificationType.Waiting: return __style.waitingIcon
      case MM.NotificationType.Exclamation: return __style.errorCircleIcon
      case MM.NotificationType.Check: return __style.comboBoxCheckIcon
      default: return ""
      }
    }
  }

  Text {
    id: text

    anchors.verticalCenter: parent.verticalCenter
    anchors.left: leftIcon.right
    width: parent.width - 60 * __dp - closeButton.width - leftIcon.width
    text: model.message
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
      case MM.NotificationType.Information: return __style.deepOceanColor
      case MM.NotificationType.Success: return __style.forestColor
      case MM.NotificationType.Warning: return __style.earthColor
      case MM.NotificationType.Error: return __style.grapeColor
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
      onClicked: __notificationModel.remove(model.id)
    }
  }

  Behavior on scale { NumberAnimation { easing.type: Easing.OutCubic; from: 0; to: 1.0; duration: 200 } }
}
