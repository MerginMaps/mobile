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

Item {

  Rectangle {
    anchors.fill: parent
    color: InputStyle.clrPanelMain
  }

  Image {
    id: logo
    anchors.centerIn: parent
    source: InputStyle.mmLogoVertical
    width: parent.width/2
    sourceSize.height: 0
    fillMode: Image.PreserveAspectFit
    sourceSize.width: width
  }

  Text {
    text: qsTr("Opening project ...")
    anchors.top: logo.bottom
    anchors.topMargin: InputStyle.smallGap
    anchors.horizontalCenter: parent.horizontalCenter
    font.pixelSize: InputStyle.fontPixelSizeNormal
    color: InputStyle.fontColor
  }
}
