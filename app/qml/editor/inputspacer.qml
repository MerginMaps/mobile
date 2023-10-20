/***************************************************************************
 checkbox.qml
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import "../components"

Rectangle {
  id: spacer

  property bool isHLine: value
  property int spacerHeight: customStyle.fields.height
  property int hLineWidth: 1

  height: isHLine ? hLineWidth : spacerHeight
  width: parent.width
  color: isHLine ? customStyle.fields.fontColor : "transparent"
  anchors {
    right: parent.right
    left: parent.left
  }
}
