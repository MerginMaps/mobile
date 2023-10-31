/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import "../Style.js" as Style
import "."

Item {
  id: control

  width: height
  height: Style.mapItemHeight

  property alias iconSource: icon.source

  signal clicked

  Rectangle {
    width: parent.width
    height: parent.height
    radius: control.height / 2
    color: Style.white

    layer.enabled: true
    layer.effect: MMShadow {}

    MMIcon {
      id: icon

      anchors.centerIn: parent
      color: Style.forest
    }

    MouseArea {
      anchors.fill: parent
      onClicked: control.clicked()
    }
  }
}
