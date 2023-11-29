/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import ".."

Item {
  id: control

  width: height
  height: StyleV2.mapItemHeight

  property alias iconSource: icon.source

  signal clicked

  Rectangle {
    width: parent.width
    height: parent.height
    radius: control.height / 2
    color: StyleV2.whiteColor

    layer.enabled: true
    layer.effect: MMShadow {}

    MMIcon {
      id: icon

      anchors.centerIn: parent
      color: StyleV2.forestColor
    }

    MouseArea {
      anchors.fill: parent
      onClicked: control.clicked()
    }
  }
}
