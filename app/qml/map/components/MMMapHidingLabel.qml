/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "."

MMHidingBox {
  id: root

  property string text

  function show() {
    root.visible = true
  }

  function hide() {
    root.visible = false
  }

  height: __style.row40
  timerInterval: 10000
  fadeOutDuration: 1000

  // Text
  Text {
    height: parent.height
    width: parent.width - 2 * __style.pageMargins
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter

    color: __style.forestColor
    text: root.text
    font: __style.t3
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    elide: Text.ElideRight
  }
}
