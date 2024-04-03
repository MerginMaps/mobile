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
import QtQuick.Controls.Basic

import "../" as MMComponents

Item {
  id: root

  signal clicked

  property int buttonSpacing: 5 * __dp

  property bool rotating: false
  property var iconSourceSelected
  property var iconSource
  property color iconColor
  property color iconColorDisabled
  property string text
  property bool selected

  // Show grayed out button if button is disabled or unselected button in selectable toolbar mode
  property bool useEnabledVisual: root.enabled && (iconSource === iconSourceSelected || selected)

  onRotatingChanged: {
    if (rotating)
      rotateAnimation.start()
    else
      rotateAnimation.stop()
  }

  Item {
    id: container
    width: parent.width - 10 * __dp
    height: parent.height - 10 * __dp
    anchors.centerIn: parent

    MMComponents.MMIcon {
      id: icon

      anchors.horizontalCenter: parent.horizontalCenter
      anchors.top: parent.top
      anchors.topMargin: ( container.height - (icon.height + text.height + root.buttonSpacing) ) / 2

      source: root.selected ? root.iconSourceSelected : root.iconSource
      color: root.useEnabledVisual ? root.iconColor : root.iconColorDisabled
    }

    RotationAnimation {
      id: rotateAnimation

      target: icon

      from: 0
      to: 720
      duration: 1000

      alwaysRunToEnd: true
      loops: Animation.Infinite
      easing.type: Easing.InOutSine
    }

    Text {
      id: text

      text: root.text
      color: root.useEnabledVisual ? root.iconColor : root.iconColorDisabled
      font: __style.t4
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.top: icon.bottom
      anchors.topMargin: root.buttonSpacing
      horizontalAlignment: Text.AlignHCenter
      elide: Text.ElideMiddle
    }

    MouseArea {
      anchors.fill: parent
      onClicked: root.clicked()
    }
  }
}
