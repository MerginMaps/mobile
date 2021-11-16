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
    fillMode: Image.PreserveAspectFit
    source: InputStyle.inputLogo
    sourceSize.height: 0
    sourceSize.width: width
    width: parent.width / 2
  }
  Text {
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    anchors.verticalCenterOffset: parent.height / 6
    color: "white"
    font.pixelSize: InputStyle.fontPixelSizeNormal
    text: qsTr("Opening project ...")
  }
}
