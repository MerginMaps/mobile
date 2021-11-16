/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick 2.0
import "."  // import InputStyle singleton

TabButton {
  id: button

  //property string contentText: ""
  property real horizontalAlignment: Text.AlignHCenter

  anchors.bottom: parent.bottom
  anchors.bottomMargin: 0
  anchors.top: parent.top
  anchors.topMargin: 0
  height: projectMenuButtons.height

  // overwritting TabButton default background
  background: Item {
    anchors.fill: parent
  }
  contentItem: Text {
    anchors.fill: parent
    anchors.margins: InputStyle.panelMargin
    color: InputStyle.fontColor
    font.bold: true
    font.pixelSize: InputStyle.fontPixelSizeNormal
    font.underline: button.checked
    horizontalAlignment: button.horizontalAlignment
    opacity: button.checked ? 1 : 0.7
    text: button.text
    verticalAlignment: Text.AlignVCenter
  }
}
