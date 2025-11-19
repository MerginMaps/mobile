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

  property real minHeight: 66 * __dp

  height: Math.max(minHeight, text.height + 2 * 15 * __dp)

  anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined

  readonly property int innerSpacing: 5 * __dp

  radius: __style.radius12
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
    size: __style.icon24
    color: text.color
    visible: model.icon !== MM.NotificationType.NoneIcon
    source: {
      switch( model.icon ) {
      case MM.NotificationType.NoneIcon: return ""
      case MM.NotificationType.WaitingIcon: return __style.waitingIcon
      case MM.NotificationType.InfoIcon: return __style.infoIcon
      case MM.NotificationType.ExclamationIcon: return __style.errorCircleIcon
      case MM.NotificationType.CheckIcon: return __style.doneCircleIcon
      default: return ""
      }
    }
  }

  RotationAnimation {
    target: leftIcon

    from: 0
    to: 360

    duration: 1800

    running: model.icon === MM.NotificationType.WaitingIcon
    loops: Animation.Infinite
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
    textFormat: Text.RichText
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

  MouseArea {
    anchors.fill: parent
    onClicked: __notificationModel.onNotificationClicked(model.id)
  }

  MMRoundButton {
    id: closeButton

    anchors {
      right: parent.right
      verticalCenter: parent.verticalCenter
      rightMargin: 20 * __dp
    }
    iconSource: __style.closeIcon
    iconColor: text.color
    bgndColor: __style.transparentColor
    bgndHoverColor: __style.transparentColor

    onClicked: __notificationModel.remove(model.id)
  }

  Behavior on scale { NumberAnimation { easing.type: Easing.OutCubic; from: 0; to: 1.0; duration: 200 } }
}
