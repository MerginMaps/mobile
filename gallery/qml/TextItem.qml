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

import "../app/qml/components"
import "../"

Column {
  id: root

  property alias font: textArea.font
  property alias testText: textArea.text
  property alias text: fontName.text

  Text { id: fontName; width: 250 * __dp; font.bold: true }
  Item{ width: parent.width; height: 1 }
  Rectangle {
    width: textArea.width
    height: textArea.height
    color: "white"

    MMText {
      id: textArea
      font: root.font
    }
  }

  Item{ width: parent.width; height: 15 }
}
