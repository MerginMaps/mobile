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
import "../"  // import InputStyle singleton

Rectangle {
  id: header
  property bool backIconVisible: true
  property string backText: qsTr("Cancel")
  property bool backTextVisible: false
  property color fontBtnColor: InputStyle.fontColorBright
  property real rowHeight
  property string titleText: ""
  property bool withBackButton: true

  color: InputStyle.clrPanelMain

  signal back

  Item {
    id: backButton
    anchors.left: header.left
    anchors.leftMargin: InputStyle.panelMargin
    height: header.rowHeight
    visible: withBackButton
    width: height * 2
    z: title.z + 1

    Image {
      id: image
      anchors.bottomMargin: (parent.height - height) / 2
      anchors.left: parent.left
      anchors.topMargin: anchors.bottomMargin
      anchors.verticalCenter: parent.verticalCenter
      fillMode: Image.PreserveAspectFit
      height: backIconVisible ? InputStyle.fontPixelSizeNormal : 0
      source: InputStyle.backIcon
      sourceSize.height: height
      sourceSize.width: width
      visible: false
      width: height
    }
    ColorOverlay {
      anchors.fill: image
      color: InputStyle.fontColorBright
      source: image
      visible: header.backIconVisible
    }
    Text {
      id: backButtonText
      anchors.bottom: parent.bottom
      anchors.left: image.right
      anchors.leftMargin: backIconVisible ? header.rowHeight / 4 : 0
      anchors.right: parent.right
      anchors.top: parent.top
      color: header.fontBtnColor
      font.pixelSize: InputStyle.fontPixelSizeNormal
      height: header.rowHeight
      horizontalAlignment: Text.AlignLeft
      text: header.backText
      verticalAlignment: Text.AlignVCenter
      visible: backTextVisible
    }
    MouseArea {
      anchors.fill: parent
      anchors.leftMargin: -InputStyle.panelMargin

      onClicked: back()
    }
  }
  Text {
    id: title
    anchors.fill: parent
    color: InputStyle.fontColor
    font.bold: true
    font.pixelSize: InputStyle.fontPixelSizeTitle
    horizontalAlignment: Text.AlignHCenter
    text: header.titleText
    verticalAlignment: Text.AlignVCenter
  }
}
