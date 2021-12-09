/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import ".."

Item {
  id: root

  property double maxWidth
  property alias content: contentContainer.children

  signal clicked()

  height: InputStyle.mapBtnHeight
  width: childrenRect.width

  Rectangle {
    id: rect

    height: parent.height

    border.width: InputStyle.borderSize
    border.color: InputStyle.darkGreen

    implicitWidth: Math.min( contentContainer.childrenRect.width, root.maxWidth ) + InputStyle.smallGap

    radius: InputStyle.cornerRadius
    color: InputStyle.panelBackgroundLight

    layer.enabled: true
    layer.effect: Shadow { verticalOffset: 0; horizontalOffset: 0; radius: 7; samples: 15 }

    Item {
      id: contentContainer
      // component placeholder, component passed via content property will be child of this element

      height: parent.height
      anchors.horizontalCenter: parent.horizontalCenter
    }

    MouseArea {
      anchors.fill: parent
      onClicked: root.clicked()
    }
  }
}
