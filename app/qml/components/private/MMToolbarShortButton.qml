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

  property bool isSelectableButton: iconSource.toString() !== iconSourceSelected.toString()
  property bool useDisabledVisual: !root.enabled || (isSelectableButton && !selected)

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
      color: root.useDisabledVisual ?  root.iconColorDisabled : root.iconColor
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

      width: parent.width
      text: root.text
      color: root.useDisabledVisual ? root.iconColorDisabled : root.iconColor
      font: lineCount === 1 ? __style.t4 : __style.t5
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.top: icon.bottom
      anchors.topMargin: root.buttonSpacing
      horizontalAlignment: Text.AlignHCenter
      elide: Text.ElideRight
      wrapMode: Text.Wrap
      maximumLineCount: 2
    }

    MouseArea {
      anchors.fill: parent
      onClicked: root.clicked()
    }
  }
}
