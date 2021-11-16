/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.9
import QtQuick.Controls 2.2

Item {

  Rectangle {
    anchors.fill: parent
    color: InputStyle.fontColor
  }

  Image {
    id: logo
    anchors.centerIn: parent
    source: InputStyle.inputLogo
    width: parent.width/2
    sourceSize.height: 0
    fillMode: Image.PreserveAspectFit
    sourceSize.width: width
  }

  Text {
    text: qsTr("Opening project ...")
    anchors.verticalCenterOffset: parent.height/6
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    font.pixelSize: InputStyle.fontPixelSizeNormal
    color: "white"
  }

}
