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
  property bool showIcon: false
  property string actionText: ""
  property bool showClose: false

  signal actionClicked()
  signal closeClicked()

  color: __style.informativeColor
  radius: __style.radius8
  implicitHeight: bannerText.implicitHeight + 2 * __style.margin12

  MMComponents.MMIcon {
    id: infoIcon

    visible: root.showIcon

    anchors.left: parent.left
    anchors.leftMargin: __style.margin12
    anchors.verticalCenter: parent.verticalCenter

    source: __style.infoIcon
    color: __style.deepOceanColor
    size: __style.icon24
  }

  MMComponents.MMText {
    id: bannerText

    anchors.left: infoIcon.visible ? infoIcon.right : parent.left
    anchors.leftMargin: infoIcon.visible ? __style.spacing8 : __style.margin12
    anchors.right: actionButton.visible ? actionButton.left : ( closeButton.visible ? closeButton.left : parent.right )
    anchors.rightMargin: ( actionButton.visible || closeButton.visible ) ? __style.spacing8 : __style.margin12
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
    anchors.rightMargin: __style.margin12
    anchors.verticalCenter: parent.verticalCenter

    onClicked: root.actionClicked()
  }

  MMComponents.MMRoundButton {
    id: closeButton

    visible: root.showClose

    iconSource: __style.closeIcon
    iconColor: __style.deepOceanColor
    bgndColor: __style.transparentColor
    bgndHoverColor: __style.transparentColor

    anchors.right: parent.right
    anchors.rightMargin: __style.margin4
    anchors.verticalCenter: parent.verticalCenter

    onClicked: root.closeClicked()
  }
}
