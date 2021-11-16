/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import "../"  // import InputStyle singleton

Rectangle {
  id: root
  property color backgroundColor: InputStyle.fontColor
  property bool enabled: true
  property bool faded: false
  property color fontColor: "white"
  property bool handleClicks: true // enable property is used also for color
  property string imageSource: ""
  property string imageSource2: ""
  property bool imageSourceCondition: true
  property bool isHighlighted: false
  property string text

  anchors.centerIn: parent
  color: root.isHighlighted ? fontColor : backgroundColor
  height: width
  opacity: root.faded ? 0.6 : 1

  signal activated
  signal activatedOnHold

  MouseArea {
    anchors.fill: parent
    enabled: root.enabled && handleClicks

    onClicked: {
      root.activated();
    }
    onPressAndHold: {
      root.activatedOnHold();
    }
  }
  Image {
    id: image
    anchors.fill: parent
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.margins: root.width / 4
    anchors.topMargin: 0
    fillMode: Image.PreserveAspectFit
    source: root.imageSourceCondition ? root.imageSource : root.imageSource2
    sourceSize.height: height
    sourceSize.width: width
    visible: source
  }
  ColorOverlay {
    anchors.fill: image
    color: {
      if (!root.enabled)
        return InputStyle.fontColorBright;
      return root.isHighlighted ? backgroundColor : fontColor;
    }
    source: image
  }
  Text {
    anchors.fill: parent
    color: {
      if (!root.enabled)
        return InputStyle.fontColorBright;
      return root.isHighlighted ? backgroundColor : fontColor;
    }
    font.bold: true
    font.pixelSize: InputStyle.fontPixelSizeSmall
    horizontalAlignment: Text.AlignHCenter
    text: root.text
    verticalAlignment: Text.AlignBottom
    z: image.z + 1
  }
}
