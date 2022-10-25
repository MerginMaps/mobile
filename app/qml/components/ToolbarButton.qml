/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

import ".."

Rectangle {
  id: root

  property string imageSource
  property string text

  property color fontColor: InputStyle.fontColorWhite
  property color backgroundColor: InputStyle.clrPanelBackground

  signal clicked()

  height: InputStyle.toolbarButtonSize
  width: InputStyle.toolbarButtonSize

  color: backgroundColor

  MouseArea {
    anchors {
      fill: parent // make the click area bigger
      leftMargin: -InputStyle.buttonClickArea
      topMargin: -InputStyle.buttonClickArea
      rightMargin: -InputStyle.buttonClickArea
      bottomMargin: -InputStyle.buttonClickArea
    }

    onClicked: root.clicked()
  }

  Column {
    anchors.fill: parent

    Item {
      width: parent.width
      height: parent.height * 0.65

      Image {
        anchors.centerIn: parent

        source: root.imageSource

        width: InputStyle.iconSizeLarge
        height: InputStyle.iconSizeLarge
        sourceSize.width: width
        sourceSize.height: height

        fillMode: Image.PreserveAspectFit
      }
    }

    Text {
      width: parent.width

      text: root.text
      color: root.fontColor

      font.bold: true
      font.pixelSize: InputStyle.fontPixelSizeSmall

      verticalAlignment: Text.AlignBottom
      horizontalAlignment: Text.AlignHCenter
    }
  }
}
