/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../../components" as MMComponents

Rectangle {
  id: root

  property string text
  property string actionText: ""

  signal actionClicked()

  color: __style.informativeColor
  radius: __style.radius8
  implicitHeight: bannerText.implicitHeight + 2 * __style.margin8

  MMComponents.MMText {
    id: bannerText

    anchors.left: parent.left
    anchors.leftMargin: __style.margin12
    anchors.right: actionButton.visible ? actionButton.left : parent.right
    anchors.rightMargin: actionButton.visible ? __style.spacing8 : __style.margin12
    anchors.verticalCenter: parent.verticalCenter

    text: root.text
    font: __style.t4
    color: __style.deepOceanColor
    wrapMode: Text.Wrap
    elide: Text.ElideNone
  }

  MMComponents.MMButton {
    id: actionButton

    visible: root.actionText !== ""

    type: MMComponents.MMButton.Types.Tertiary
    size: MMComponents.MMButton.Sizes.ExtraSmall
    text: root.actionText
    fontColor: __style.skyColor
    bgndColor: __style.deepOceanColor

    anchors.right: parent.right
    anchors.rightMargin: __style.margin8
    anchors.verticalCenter: parent.verticalCenter

    onClicked: root.actionClicked()
  }
}
