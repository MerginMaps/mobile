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
  id: root

  signal clicked

  required property var iconSource
  property color iconColor: __style.polarColor
  property color disabledIconColor: __style.mediumGreenColor
  required property string text
  property string menuButtonRightText: ""

  property var parentMenu // drawer to close
  property var parentToolbar
  property bool isMenuButton: false
  property bool visibilityMode: true // set instead of "visibility" to show/hide from MMToolbar

  property alias buttonIcon: icon
  property int buttonSpacing: 5 * __dp

  onVisibilityModeChanged: if (root.parentToolbar !== undefined) root.parentToolbar.setupBottomBar()

  height: isMenuButton ? __style.menuDrawerHeight/2 : __style.toolbarHeight

  // Toolbar button
  Item {
    id: container
    width: parent.width - 10 * __dp
    height: parent.height - 10 * __dp
    anchors.centerIn: parent
    visible: !root.isMenuButton

    MMIcon {
      id: icon

      anchors.horizontalCenter: parent.horizontalCenter
      anchors.top: parent.top
      anchors.topMargin: ( container.height - (icon.height + text.height + root.buttonSpacing) ) / 2

      source: root.iconSource
      color: root.enabled ? root.iconColor : root.disabledIconColor
    }

    Text {
      id: text

      text: root.text
      color: root.enabled ? root.iconColor : root.disabledIconColor
      font: __style.t4
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.top: icon.bottom
      anchors.topMargin: root.buttonSpacing
      horizontalAlignment: Text.AlignHCenter
      elide: Text.ElideMiddle
    }

    MouseArea {
      anchors.fill: parent
      enabled: !root.isMenuButton && root.enabled
      onClicked: root.clicked()
    }
  }

  // Menu button
  MMToolbarMenuButton {
    id: menuButton

    width: root.width
    height: __style.menuDrawerHeight
    visible: root.isMenuButton
    enabled: root.enabled
    iconSource: root.iconSource
    text: root.text
    rightText: root.menuButtonRightText
    onClicked: {
      parentMenu.close()
      root.clicked()
    }
  }
}
