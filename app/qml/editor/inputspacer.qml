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

  /*required*/ property var config: parent.config

  property bool isHLine: config["IsHLine"]

  height: 1 // parent form's list inserts space between each 2 elements
  width: parent.width
  color: isHLine ? customStyle.fields.backgroundColor : "transparent"
  anchors {
    right: parent.right
    left: parent.left
  }
}
