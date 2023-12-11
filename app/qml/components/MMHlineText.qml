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
import QtQuick.Layouts

RowLayout {

  required property var title

  id: root
  spacing: 10

  Rectangle {
    id: topRect
    width: 50
    height: 1
    color: __style.forestColor
  }

  Label {
    text: title
    font: __style.p5
    color: topRect.color
  }

  Rectangle {
    width: topRect.width
    height: topRect.height
    color: topRect.color
  }
}
