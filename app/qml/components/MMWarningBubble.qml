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

  required property string title
  property string description
  property alias imageSource: icon.source

  signal clicked

  Rectangle {
    width: root.width
    height: row.height
    color: __style.nightColor
    radius: __style.radius12
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

    Column {
      width: root.width - icon.width - 4 * row.spacing - 2 * row.padding

      Text {
        width: parent.width
        height: root.description.length > 0 ? icon.height / 2 : icon.height
        text: root.title
        font: __style.t3
        color: __style.whiteColor
        wrapMode: Label.WordWrap
        lineHeight: 1.5
        maximumLineCount: root.description.length > 0 ? 1 : 2
        verticalAlignment: Text.AlignVCenter
        elide: root.description.length > 0 ? Text.ElideRight : Text.ElideNone
        textFormat: Text.RichText
      }

      Text {
        width: parent.width
        height: root.description.length > 0 ? icon.height / 2 : 0
        text: root.description
        font: __style.p6
        color: __style.whiteColor
        wrapMode: Label.WordWrap
        lineHeight: 1.5
        maximumLineCount: root.title.length > 0 ? 1 : 2
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
        textFormat: Text.RichText
      }
    }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: root.clicked()
  }
}
