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
import "../Style.js" as Style

Item {
  id: control

  signal clicked

  required property var iconSource
  required property string text
  property var type: MMToolbarButton.Button.Normal
  property bool isMenuButton: false

  enum Button { Normal, Save }

  height: isMenuButton ? Style.menuDrawerHeight/2 : Style.toolbarHeight

  Rectangle {
    width: parent.width - Style.commonSpacing/2
    height: parent.height - Style.commonSpacing/2
    anchors.centerIn: parent
    clip: control.type !== MMToolbarButton.Button.Save
    color: Style.transparent
    visible: !control.isMenuButton

    Image {
      id: icon

      source: control.iconSource
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: parent.bottom
      anchors.bottomMargin: 2 * Style.commonSpacing + (control.type === MMToolbarButton.Button.Save ? 14 * __dp : 0)

      Rectangle {
        visible: control.type === MMToolbarButton.Button.Save
        anchors.centerIn: parent
        width: 56 * __dp
        height: width
        radius: width / 2
        color: Style.transparent
        border.color: Style.grass
        border.width: 14 * __dp
      }
    }
    Text {
      id: text

      text: control.text
      width: parent.width
      color: Style.white
      font: Qt.font(Style.t4)
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: parent.bottom
      anchors.bottomMargin: Style.commonSpacing
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
    height: Style.menuDrawerHeight
    visible: control.isMenuButton
    iconSource: control.iconSource
    text: control.text
    onClicked: control.clicked()
  }
}
