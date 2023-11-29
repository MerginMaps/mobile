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
import ".."

Item {
  id: control

  signal clicked

  required property var iconSource
  required property string text
  property var type: MMToolbarButton.Button.Normal
  property bool isMenuButton: false

  enum Button { Normal, Save }

  height: isMenuButton ? styleV2.menuDrawerHeight/2 : styleV2.toolbarHeight

  StyleV2 { id: styleV2 }

  Rectangle {
    width: parent.width - 10 * __dp
    height: parent.height - 10 * __dp
    anchors.centerIn: parent
    clip: control.type !== MMToolbarButton.Button.Save
    color: styleV2.transparentColor
    visible: !control.isMenuButton

    Image {
      id: icon

      source: control.iconSource
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: parent.bottom
      anchors.bottomMargin: 40 * __dp + (control.type === MMToolbarButton.Button.Save ? 14 * __dp : 0)

      Rectangle {
        visible: control.type === MMToolbarButton.Button.Save
        anchors.centerIn: parent
        width: 56 * __dp
        height: width
        radius: width / 2
        color: styleV2.transparentColor
        border.color: styleV2.grassColor
        border.width: 14 * __dp
      }
    }
    Text {
      id: text

      text: control.text
      width: parent.width
      color: styleV2.whiteColor
      font: styleV2.t4
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: parent.bottom
      anchors.bottomMargin: 20 * __dp
      horizontalAlignment: Text.AlignHCenter
    }

    MouseArea {
      anchors.fill: parent
      onClicked: control.clicked()
    }
  }

  // Menu button
  MMToolbarMenuButton {
    width: control.width
    height: styleV2.menuDrawerHeight
    visible: control.isMenuButton
    iconSource: control.iconSource
    text: control.text
    onClicked: control.clicked()
  }
}
