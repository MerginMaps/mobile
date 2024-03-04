/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick

import "../../components"
import "../../inputs"

Item {
  id: root

  height: leftItem.height

  signal linkClicked

  required property string title
  property string desc: ""
  required property url iconSource
  property int notificationCount: 0

  Row {
    id: leftItem
    spacing: 8 * __dp
    anchors.left: parent.left

    Rectangle {
      anchors.verticalCenter: parent.verticalCenter
      id: leftIcon
      width: 50 * __dp
      height: width
      radius: width / 2
      color: __style.whiteColor

      MMIcon {
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        source: root.iconSource
        size: __style.icon24
        color: __style.forestColor
      }
    }

    Column {
      id: descItems
      anchors.verticalCenter: parent.verticalCenter

      Text {
        text: root.title
        color: __style.nightColor
        font: __style.t3
      }

      Text {
        text: root.desc
        visible: root.desc !== ""
        color: __style.nightColor
        font: __style.p6
      }
    }
  }

  Row {
    id: rightItem
    height: leftItem.height
    anchors.right: parent.right
    spacing: 8 * __dp

    Rectangle {
      id: notificationRect
      anchors.verticalCenter: parent.verticalCenter
      visible: root.notificationCount > 0
      width: 24 * __dp
      height: width
      radius: width / 2
      color: __style.forestColor

      Text {
        text: root.notificationCount
        anchors.fill: parent
        color: __style.whiteColor
        font: __style.t4
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
      }
    }

    MMIcon {
      anchors.verticalCenter: parent.verticalCenter
      id: rightIcon
      source: __style.arrowLinkRightIcon
      color: __style.forestColor
    }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: parent.linkClicked()
  }
}
