/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.0
import "."  // import InputStyle singleton

Rectangle {
  id: root
  property bool bold: false
  property string text: ""
  property string text2: ""

  color: InputStyle.clrPanelMain
  height: InputStyle.rowHeight
  width: parent.width

  Text {
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.leftMargin: InputStyle.panelMargin
    anchors.right: parent.right
    anchors.top: parent.top
    color: InputStyle.fontColor
    elide: Text.ElideRight
    font.bold: bold
    font.pixelSize: InputStyle.fontPixelSizeNormal
    horizontalAlignment: Text.AlignLeft
    text: root.text
    verticalAlignment: Text.AlignVCenter
  }
  Text {
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.leftMargin: InputStyle.panelMargin
    anchors.right: parent.right
    anchors.rightMargin: InputStyle.panelMargin
    anchors.top: parent.top
    color: InputStyle.fontColor
    elide: Text.ElideRight
    font.bold: bold
    font.pixelSize: InputStyle.fontPixelSizeNormal
    horizontalAlignment: Text.AlignRight
    text: text2
    verticalAlignment: Text.AlignVCenter
    visible: text2
  }
}
