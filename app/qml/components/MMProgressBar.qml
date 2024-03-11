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

Rectangle {
  id: control

  required property real position // [0 - 1]
  property color progressColor: __style.grassColor

  width: parent.width
  height: 12 * __dp
  color: __style.lightGreenColor
  radius: height / 2

  Rectangle {
    width: control.position > 1 ? parent.width : control.position * parent.width
    height: parent.height
    color: control.progressColor
    radius: height / 2

    Behavior on width {
      NumberAnimation { duration: 150 }
    }
  }
}
