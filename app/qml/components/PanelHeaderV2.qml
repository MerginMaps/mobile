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

import ".."

Rectangle {
  id: root

  property string headerTitle

  signal backClicked()

  height: InputStyle.rowHeightHeader
  // set width manually

  color: InputStyle.clrPanelMain

  Text {
    id: title

    anchors.fill: parent

    text: root.headerTitle

    font.bold: true
    color: InputStyle.fontColor
    font.pixelSize: InputStyle.fontPixelSizeHeader

    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
  }

  Image {
    id: backicon

    anchors {
      left: parent.left
      leftMargin: InputStyle.panelMargin
      verticalCenter: parent.verticalCenter
    }

    source: InputStyle.backIconV2
    sourceSize.width: InputStyle.closeBtnSize
    sourceSize.height: InputStyle.closeBtnSize

    MouseArea {
      anchors {
        fill: parent // make the click area bigger
        leftMargin: -InputStyle.buttonClickArea
        topMargin: -InputStyle.buttonClickArea
        rightMargin: -InputStyle.buttonClickArea
        bottomMargin: -InputStyle.buttonClickArea
      }

      onClicked: root.backClicked()
    }
  }
}
