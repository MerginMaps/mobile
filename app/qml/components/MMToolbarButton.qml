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
  required property string text

  property alias buttonIcon: icon

  property var type: MMToolbarButton.Button.Normal
  property bool isMenuButton: false

  property int buttonSpacing: control.type === MMToolbarButton.Button.Emphasized ? 7 * __dp : 5 * __dp
  enum Button { Normal, Emphasized }

  height: isMenuButton ? __style.menuDrawerHeight/2 : __style.toolbarHeight

  // Toolbar button
  Rectangle {
    id: container
    width: parent.width - 10 * __dp
    height: parent.height - 10 * __dp
    anchors.centerIn: parent
    visible: !control.isMenuButton

    color: __style.transparentColor

    Rectangle {
      id: iconBg
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.top: parent.top
      anchors.topMargin: ( container.height - (iconBg.height + text.height + control.buttonSpacing) ) / 2

      width: control.height * 0.5
      height: width
      radius: width / 2
      color: control.type === MMToolbarButton.Button.Emphasized ? __style.grassColor :  __style.transparentColor

      MMIcon {
        id: icon
        useCustomSize: true
        width: parent.width
        height: parent.height
        source: control.iconSource
        anchors.centerIn: parent
        }
      }

      Text {
        id: text

        text: control.text
        color: __style.whiteColor
        font: __style.t4
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: iconBg.bottom
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
    width: control.width
    height: __style.menuDrawerHeight
    visible: control.isMenuButton
    iconSource: control.iconSource
    text: control.text
    onClicked: control.clicked()
  }
}
