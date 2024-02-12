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

Item {
  id: root

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  Image {
    id: logo
    anchors.centerIn: parent
    source: __style.mmSymbol
    width: 60 * __dp
    height: width
    sourceSize.height: 0
    fillMode: Image.PreserveAspectFit
    sourceSize.width: width
  }

  Text {
    text: qsTr("Opening project ...")
    anchors.bottom: root.bottom
    anchors.bottomMargin: 32 * __dp
    anchors.horizontalCenter: parent.horizontalCenter
    font: __style.t1
    color: __style.forestColor
  }
}
