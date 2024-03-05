/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts

import "../../components"

Item {
  id: root

  property string title
  property string desc: ""
  property url iconSource
  property int notificationCount: 0

  signal clicked

  implicitHeight: 50 * __dp

  RowLayout {

    height: parent.height
    width: parent.width

    spacing: __style.spacing12

    Rectangle {
      width: parent.height
      height: parent.height

      radius: width

      color: __style.whiteColor

      MMIcon {
        anchors.centerIn: parent
        source: root.iconSource

        size: __style.icon24
        color: __style.forestColor
      }
    }

    Column {
      Layout.fillWidth: true
      Layout.preferredHeight: parent.height
      Layout.alignment: Qt.AlignVCenter

      Text {
        height: descriptionText.visible ? 26 * __dp : parent.height
        width: parent.width

        text: root.title

        font: __style.t3
        elide: Text.ElideRight
        color: __style.nightColor
        verticalAlignment: Text.AlignVCenter
      }

      Text {
        id: descriptionText

        height: 24 * __dp
        width: parent.width

        text: root.desc

        visible: root.desc !== ""

        font: __style.p6
        elide: Text.ElideRight
        color: __style.nightColor
        verticalAlignment: Text.AlignVCenter
      }
    }

    Rectangle {

      Layout.preferredWidth: 24 * __dp
      Layout.preferredHeight: width

      radius: width
      color: __style.forestColor

      visible: root.notificationCount > 0

      Text {
        anchors.fill: parent

        text: root.notificationCount

        font: __style.t4
        color: __style.whiteColor

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
      }
    }

    MMIcon {
      source: __style.arrowLinkRightIcon
      color: __style.forestColor
    }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: root.clicked()
  }
}
