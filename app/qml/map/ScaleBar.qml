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
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "../"  // import InputStyle singleton

Item {
  id: scaleBar
  property color barColor: InputStyle.fontColor
  property string barText: scaleBarKit.distance + " " + scaleBarKit.units
  property int barWidth: scaleBarKit.width
  property int lineWidth: 2 * QgsQuick.Utils.dp
  property alias mapSettings: scaleBarKit.mapSettings
  property alias preferredWidth: scaleBarKit.preferredWidth

  width: barWidth

  onVisibleChanged: {
    if (scaleBar.visible) {
      fadeOut.stop();
      scaleBarTimer.restart();
    }
  }

  ScaleBarKit {
    id: scaleBarKit
  }
  Rectangle {
    id: background
    color: InputStyle.clrPanelMain
    height: parent.height
    opacity: 0.25
    radius: 5
    width: parent.width
  }
  Item {
    anchors.fill: parent
    anchors.leftMargin: 5 * QgsQuick.Utils.dp
    anchors.rightMargin: anchors.leftMargin

    Text {
      id: text
      anchors.fill: parent
      color: barColor
      font.pixelSize: InputStyle.fontPixelSizeSmall
      horizontalAlignment: Text.AlignHCenter
      text: barText
      verticalAlignment: Text.AlignTop
    }
    Rectangle {
      id: leftBar
      anchors.right: parent.right
      anchors.rightMargin: 0
      color: barColor
      height: scaleBar.height / 3
      opacity: 1
      width: scaleBar.lineWidth
      y: (scaleBar.height - leftBar.height) / 2
    }
    Rectangle {
      id: bar
      anchors.left: parent.left
      anchors.right: parent.right
      color: barColor
      height: scaleBar.lineWidth
      y: (scaleBar.height - scaleBar.lineWidth) / 2
    }
    Rectangle {
      id: rightBar
      color: barColor
      height: scaleBar.height / 3
      width: scaleBar.lineWidth
      y: (scaleBar.height - leftBar.height) / 2
    }
  }
  Timer {
    id: scaleBarTimer
    interval: 3000
    repeat: false
    running: false

    onTriggered: {
      fadeOut.start();
    }
  }

  NumberAnimation on opacity  {
    id: fadeOut
    duration: 1000
    to: 0.0

    onStopped: {
      scaleBar.visible = false;
      scaleBar.opacity = 1.0;
    }
  }
}
