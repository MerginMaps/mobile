/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts

import "../../components" as MMComponents

Rectangle {
  id: root

  property string text
  property string actionText: ""

  signal actionClicked()

  color: __style.informativeColor
  radius: __style.radius8
  implicitHeight: row.implicitHeight + 2 * __style.margin8

  RowLayout {
    id: row

    anchors {
      left: parent.left
      right: parent.right
      leftMargin: __style.margin12
      rightMargin: __style.margin8
      verticalCenter: parent.verticalCenter
    }

    spacing: __style.spacing8

    MMComponents.MMText {
      Layout.fillWidth: true
      Layout.alignment: Qt.AlignVCenter

      text: root.text
      font: __style.t4
      color: __style.deepOceanColor
      wrapMode: Text.Wrap
      elide: Text.ElideNone
    }

    MMComponents.MMButton {
      id: actionButton

      visible: root.actionText !== ""
      Layout.alignment: Qt.AlignVCenter

      type: MMComponents.MMButton.Types.Tertiary
      size: MMComponents.MMButton.Sizes.ExtraSmall
      text: root.actionText
      fontColor: __style.skyColor
      bgndColor: __style.deepOceanColor

      onClicked: root.actionClicked()
    }
  }
}
