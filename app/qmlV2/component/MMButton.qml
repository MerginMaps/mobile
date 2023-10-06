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
import "../Style.js" as Style
import "."

Button {
  id: control

  property bool transparent: false

  contentItem: Text {
    anchors.centerIn: control
    font: Qt.font(Style.t3)
    text: control.text
    leftPadding: 32 * __dp
    rightPadding: 32 * __dp
    topPadding: 10 * __dp
    bottomPadding: 10 * __dp
    color: control.enabled ? control.down || control.hovered ? Style.grass : Style.forest : Style.forest
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    elide: Text.ElideRight
  }

  background: Rectangle {
    color: transparent ? "transparent" : control.enabled ? control.down || control.hovered ? Style.forest : Style.grass : Style.mediumGreen
    radius: height / 2
  }

//  onPressed: clickTransition.running = true

//  SequentialAnimation {
//    id: clickTransition

//    PropertyAnimation {
//      target: control
//      properties: "scale"
//      from: 1
//      to: 0.9
//      duration: 100
//    }

//    PropertyAnimation {
//      target: control
//      properties: "scale"
//      from: 0.9
//      to: 1
//      duration: 100
//    }
//  }
}
