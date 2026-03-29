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

CheckBox {
  id: control

  property string sourceIcon: ""
  property string description: ""
  property bool showBorder: false
  property bool small: false

  height: (description !== "" ? 96 : (control.small ? 50 : 80)) * __dp

  leftPadding: (description !== "" ? iconBgRectangle.x : 0) + iconBgRectangle.width + 30 * __dp
  rightPadding: 20 * __dp

  indicator: Rectangle {
    id: iconBgRectangle
    width: (control.small ? 24 : (control.description !== "" ? 50 : 40)) * __dp
    height: (control.small ? 24 : (control.description !== "" ? 50 : 40)) * __dp
    x: 20 * __dp
    y: control.height / 2 - height / 2
    radius: width / 2
    color: control.checked ? __style.polarColor : __style.lightGreenColor

    MMIcon {
      size: control.small ? __style.icon16 : __style.icon24
      anchors.centerIn: parent
      source: control.sourceIcon
      color: __style.forestColor
    }
  }

  contentItem: Item {
    implicitWidth: titleText.implicitWidth

    Text {
      id: titleText
      visible: control.description === ""
      width: parent.width
      height: parent.height
      text: control.text
      font: __style.t3
      color: control.checked ? __style.polarColor : __style.nightColor
      elide: Text.ElideRight
      verticalAlignment: Text.AlignVCenter
    }

    Column {
      id: textColumn
      visible: control.description !== ""
      anchors.verticalCenter: parent.verticalCenter
      width: parent.width
      spacing: 10 * __dp

      Text {
        width: parent.width
        text: control.text
        font: __style.t3
        color: control.checked ? __style.polarColor : __style.nightColor
        elide: Text.ElideRight
      }

      Text {
        width: parent.width
        text: control.description
        font: __style.p6
        color: control.checked ? __style.polarColor : __style.nightColor
        elide: Text.ElideRight
      }
    }
  }

  background: Rectangle {
    radius: __style.radius12
    color: control.checked ? __style.forestColor : __style.polarColor
    border.color: showBorder ? ( control.checked ? __style.transparentColor : __style.mediumGreenColor ) : __style.transparentColor
  }
}
