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

import "."

Rectangle {
  id: root

  enum Variant { Informative, Warning }

  property int variant: MMListBanner.Informative

  property string text
  property string actionText: ""

  signal actionClicked()

  color: variant === MMListBanner.Warning ? __style.warningColor : __style.informativeColor
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

    spacing: __style.spacing10

    MMText {
      Layout.fillWidth: true
      Layout.alignment: Qt.AlignVCenter

      text: root.text
      font: __style.t4
      color: __style.deepOceanColor
      wrapMode: Text.Wrap
      elide: Text.ElideNone
    }

    MMButton {
      id: actionButton

      visible: root.actionText !== ""
      Layout.alignment: Qt.AlignVCenter

      type: MMButton.Types.Tertiary
      size: MMButton.Sizes.ExtraSmall
      text: root.actionText
      fontColor: root.variant === MMListBanner.Warning ? __style.polarColor : __style.skyColor
      bgndColor: root.variant === MMListBanner.Warning ? __style.earthColor : __style.deepOceanColor
      bgndColorHover: bgndColor
      fontColorHover: fontColor

      onClicked: root.actionClicked()
    }
  }
}
