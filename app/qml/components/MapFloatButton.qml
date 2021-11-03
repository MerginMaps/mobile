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

  property string text
  property double maxWidth

  signal clicked()

  height: InputStyle.mapBtnHeight
  width: childrenRect.width

  Rectangle {
    id: rect

    height: parent.height

    implicitWidth: Math.min( txt.implicitWidth + InputStyle.scale( 15 ), root.maxWidth )

    radius: InputStyle.cornerRadius
    color: InputStyle.panelBackgroundLight

    Text {
      id: txt

      height: parent.height

      text: root.text
      elide: Text.ElideRight
      wrapMode: Text.NoWrap

      font.pixelSize: InputStyle.fontPixelSizeNormal
      color: InputStyle.fontColor

      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter

      anchors.horizontalCenter: parent.horizontalCenter
    }

    MouseArea {
      anchors.fill: parent
      onClicked: root.clicked()
    }
  }
}
