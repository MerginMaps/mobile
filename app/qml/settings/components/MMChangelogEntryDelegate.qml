/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import "../../components"

Item {
  id: root

  property string title
  property string datetime
  property string description

  property bool hasLine: true

  signal clicked()

  implicitHeight: childrenRect.height

  Column {
    width: parent.width
    height: childrenRect.height
    spacing: 0

    MMLine { visible: root.hasLine }

    Item { height: __style.spacing20; width: 1; visible: root.hasLine }

    Text {
      text: root.title
      wrapMode: Text.WordWrap
      width: parent.width
      height: paintedHeight
      font: __style.t1
      color: __style.forestColor
    }

    Text {
      text: root.datetime
      wrapMode: Text.WordWrap
      width: parent.width
      height: paintedHeight + __style.margin12
      font: __style.p6
      color: __style.forestColor

      visible: root.datetime
    }

    Text {
      text: root.description
      wrapMode: Text.WordWrap
      width: parent.width
      height: paintedHeight
      font: __style.p5
      color: __style.nightColor
    }

    Item { height: __style.spacing20; width: 1; visible: !root.hasLine }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: function( mouse ) {
      mouse.accepted = true
      root.clicked()
    }
  }
}
