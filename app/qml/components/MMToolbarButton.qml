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
import QtQuick.Controls.Basic

Item {
  id: control

  signal clicked

  required property var iconSource
  property color iconColor: __style.polarColor
  required property string text
  property string menuButtonRightText: ""

  property var parentMenu // drawer to close
  property alias buttonIcon: icon

  property bool isMenuButton: false
  property int buttonSpacing: 5 * __dp

  property bool visibilityMode: true

  height: isMenuButton ? __style.menuDrawerHeight/2 : __style.toolbarHeight

  // Toolbar button
  Item {
    id: container
    width: parent.width - 10 * __dp
    height: parent.height - 10 * __dp
    anchors.centerIn: parent
    visible: !control.isMenuButton

    MMIcon {
      id: icon

      anchors.horizontalCenter: parent.horizontalCenter
      anchors.top: parent.top
      anchors.topMargin: ( container.height - (icon.height + text.height + control.buttonSpacing) ) / 2

      source: control.iconSource
      color: control.iconColor
    }

    Text {
      id: text

      text: control.text
      color: __style.polarColor
      font: __style.t4
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.top: icon.bottom
      anchors.topMargin: control.buttonSpacing
      horizontalAlignment: Text.AlignHCenter
      elide: Text.ElideMiddle
    }

    MouseArea {
      anchors.fill: parent
      enabled: !control.isMenuButton
      onClicked: control.clicked()
    }
  }

  // Menu button
  MMToolbarMenuButton {
    id: menuButton

    width: control.width
    height: __style.menuDrawerHeight
    visible: control.isMenuButton
    iconSource: control.iconSource
    text: control.text
    rightText: control.menuButtonRightText
    onClicked: {
      parentMenu.close()
      control.clicked()
    }
  }
}
