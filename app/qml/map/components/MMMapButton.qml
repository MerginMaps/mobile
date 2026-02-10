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
  id: control

  width: height
  height: __style.mapItemHeight

  property alias iconSource: icon.source
  property color bgndColor: __style.polarColor

  signal clicked
  signal clickAndHold

  Rectangle {
    width: parent.width
    height: parent.height
    radius: control.height / 2
    color: control.bgndColor

    layer.enabled: true
    layer.effect: MMShadow {}

    MMIcon {
      id: icon

      anchors.centerIn: parent
      color: __style.forestColor
    }

    MouseArea {
      anchors.fill: parent
      onClicked: control.clicked()
      onPressAndHold: control.clickAndHold()
    }
  }
}
