/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7
import QtGraphicalEffects 1.0
import "../"

Rectangle {
  id: root
  property color bgColor: InputStyle.fontColorBright
  property bool fontBold: false
  property color fontColor: "white"
  property real fontPixelSize: InputStyle.fontPixelSizeNormal
  property alias icon: icon
  property string source: ""
  property string text: ""

  color: root.bgColor

  signal clicked

  MouseArea {
    anchors.fill: parent

    onClicked: {
      root.clicked();
    }
  }
  Row {
    anchors.centerIn: parent
    height: root.height
    spacing: 0

    Symbol {
      id: icon
      fontColor: root.fontColor
      height: !!source ? root.height : 0
      iconSize: height / 2
      source: root.source
      width: height
    }
    Text {
      id: label
      color: root.fontColor
      font.bold: root.fontBold
      font.pixelSize: root.fontPixelSize
      height: root.height
      horizontalAlignment: Text.AlignHCenter
      text: root.text
      verticalAlignment: Text.AlignVCenter
    }
  }
}
