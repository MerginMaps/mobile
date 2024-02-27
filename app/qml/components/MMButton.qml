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
import "."

Button {
  id: control

  property bool transparent: false

  property color colorPrimary: __style.forestColor
  property color colorSecondary: __style.grassColor
  property color colorTertiary: __style.mediumGreenColor

  contentItem: Text {
    anchors.centerIn: control
    font: __style.t3
    text: control.text
    leftPadding: 32 * __dp
    rightPadding: 32 * __dp
    topPadding: 10 * __dp
    bottomPadding: 10 * __dp
    color: control.enabled ? control.down || control.hovered ? control.colorSecondary : control.colorPrimary : control.colorPrimary
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    elide: Text.ElideRight
  }

  background: Rectangle {
    color: control.transparent ? __style.transparentColor : control.enabled ? control.down || control.hovered ? control.colorPrimary : control.colorSecondary : control.colorTertiary
    radius: height / 2
    //border: control.transparent ? control.colorPrimary : __style.transparentColor
    //border.width: control.transparent ? 1 * __dp : 0
  }
}
