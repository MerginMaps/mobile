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
import QtQuick.Controls.Basic
import "../../components"
import "../../inputs"

MMBaseInput {
  id: root

  property var parentValue: parent.value ?? ""
  property bool parentValueIsNull: parent.valueIsNull ?? false
  property bool isReadOnly: parent.readOnly ?? false

  property url photoUrl

  signal trashClicked()

  contentItemHeight: 160 * __dp
  spacing: 0
  radius: 20 * __dp

  content: MMPhoto {
    id: photo

    width: root.width
    height: root.contentItemHeight
    photoUrl: root.photoUrl

    MouseArea {
      anchors.fill: parent
      onClicked: root.contentClicked()
    }

    Rectangle {
      width: 40 * __dp
      height: width
      radius: width / 2
      color: __style.negativeColor
      anchors.right: parent.right
      anchors.bottom: parent.bottom
      anchors.rightMargin: 10 * __dp
      anchors.bottomMargin: 10 * __dp
      visible: photo.status === Image.Ready

      MMIcon {
        anchors.centerIn: parent
        source: __style.deleteIcon
        useCustomSize: true
        width: 30 * __dp
        height: width
        color: __style.grapeColor
      }

      MouseArea {
        anchors.centerIn: parent
        width: parent.width + 20 * __dp
        height: width
        onClicked: root.trashClicked()
      }
    }
  }
}
