/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls

TabBar {
  id: root

  property alias tabButtonsModel: tabBarRepeater.model

  implicitHeight: 56 * __dp

  spacing: 20 * __dp

  leftPadding: internal.tabBarPadding
  rightPadding: internal.tabBarPadding

  clip: true

  background: Rectangle {
    color: __style.lightGreenColor
  }

  Repeater {
    id: tabBarRepeater

    TabButton {
      id: tabDelegate

      property bool isSelected: TabBar.index === root.currentIndex

      height: 45 * __dp
      width: contentItem.implicitWidth

      anchors.verticalCenter: parent.verticalCenter

      focusPolicy: Qt.NoFocus

      contentItem: Text {
        text: model.Name

        leftPadding: __style.margin20
        rightPadding: __style.margin20

        font: __style.t4
        color: __style.forestColor

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
      }

      background: Rectangle {
        radius: 30 * __dp
        color: __style.grassColor

        visible: tabDelegate.isSelected
      }
    }
  }

  QtObject {
    id: internal

    property real tabBarPadding: 20 * __dp
  }
}
