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

MMBlurBox {
  id: root
  height: __style.row40

  property alias text: text.text

  timerInterval: 10000
  fadeOutDuration: 1000

  // Text
  Text {
    id: text

    height: parent.height
    width: parent.width - 2 * __style.pageMargins
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter

    color: __style.forestColor
    font: __style.t3
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    elide: Text.ElideRight
  }
}
