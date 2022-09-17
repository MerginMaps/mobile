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

import ".."

Item {
  id: root

  property string titleText
  property string text

  property alias titleComponent: title
  property alias textComponent: txt

  height: InputStyle.rowHeight

  Column {

    anchors.fill: parent

    leftPadding: InputStyle.formSpacing
    topPadding: InputStyle.formSpacing

    Label {
      id: title

      text: root.titleText

      font.pixelSize: InputStyle.fontPixelSizeSmall
      color: InputStyle.labelColor
      width: parent.width

      wrapMode: Text.Wrap
      horizontalAlignment: Text.AlignLeft
      verticalAlignment: Text.AlignVCenter
    }

    Label {
      id: txt

      text: root.text

      font.pixelSize: InputStyle.fontPixelSizeNormal
      color: InputStyle.fontColor
      width: parent.width

      wrapMode: Text.Wrap
      horizontalAlignment: Text.AlignLeft
      verticalAlignment: Text.AlignVCenter
    }
  }
}
