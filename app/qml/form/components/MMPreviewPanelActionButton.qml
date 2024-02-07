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

Rectangle {
  id: root

  property alias iconSource: icon.source
  property alias buttonText: txt.text

  signal clicked()

  width: childrenRect.width

  color: __style.lightGreenColor
  radius: 30 * __dp

  Row {
    height: parent.height

    leftPadding: 20 * __dp
    rightPadding: 20 * __dp

    spacing: 10 * __dp

    MMIcon {
      id: icon

      y: parent.height / 2 - height / 2
      width: 24 * __dp
      height: 24 * __dp

      color: __style.forestColor

      useCustomSize: true
    }

    Text {
      id: txt

      height: parent.height

      font: __style.t3
      color: __style.forestColor

      verticalAlignment: Text.AlignVCenter
    }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: function( mouse ) {
      mouse.accepted = true
      root.clicked()
    }
  }
}
