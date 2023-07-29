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

Button {
  id: control

  contentItem: Text {
    id: text

    anchors.centerIn: background
    text: control.text
    font: control.font
    leftPadding: 32
    rightPadding: 32
    topPadding: 10
    bottomPadding: 10
    color: control.down ? "#17a81a" : "#21be2b"
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    elide: Text.ElideRight
  }

  background: Rectangle {
    id: background

    width: text.width
    height: text.height
    border.color: control.down ? "red" : "#21be2b"
    border.width: 2
    radius: height
  }
}
