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

Item {
  id: root

  height: row.height

  required property string text

  Rectangle {
    width: root.width
    height: row.height
    color: __style.nightColor
    radius: __style.inputRadius
  }

  Row {
    id: row

    padding: 20 * __dp
    spacing: 10 * __dp

    Image {
      id: icon

      width: 50 * __dp
      height: width

      source: __style.warnLogoImage
    }

    Text {
      width: root.width - icon.width - 4 * row.spacing - 2 * row.padding
      height: icon.height
      text: root.text
      font: __style.t3
      color: __style.whiteColor
      wrapMode: Label.WordWrap
      lineHeight: 1.5
    }
  }
}
