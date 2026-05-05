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

CheckBox {
  id: control

  property alias sourceIcon: icon.source
  property alias description: descriptionText.text
  property bool showBorder: false
  property bool small: false

  readonly property int baseHeight: small ? 50 : 80

  height: control.baseHeight * __dp
  implicitWidth: leftPadding + titleText.implicitWidth + rightPadding

  topPadding: Math.max(0, (height - contentColumn.implicitHeight) / 2)
  bottomPadding: topPadding
  leftPadding: iconBgRectangle.x + iconBgRectangle.width + 20 * __dp
  rightPadding: 20 * __dp

  indicator: Rectangle {
    id: iconBgRectangle
    implicitWidth: (small ? 24 : 40) * __dp
    implicitHeight: (small ? 24 : 40) * __dp
    x: 20 * __dp
    y: control.height / 2 - height / 2
    radius: width / 2
    color: control.checked ? __style.polarColor : __style.lightGreenColor

    MMIcon {
      id: icon
      size: small ? __style.icon16 : __style.icon24
      anchors.centerIn: parent
      source: control.sourceIcon
      color: __style.forestColor
    }
  }

  contentItem: Column {
    id: contentColumn
    spacing: 4 * __dp

    Text {
      id: titleText
      width: parent.width
      text: control.text
      font: __style.t3
      color: control.checked ? __style.polarColor : __style.nightColor
      elide: Text.ElideRight
    }

    Text {
      id: descriptionText
      visible: text !== ""
      width: parent.width
      font: __style.p6
      color: control.checked ? __style.polarColor : __style.nightColor
      wrapMode: Text.Wrap
      maximumLineCount: 2
    }
  }

  background: Rectangle {
    radius: __style.radius12
    color: control.checked ? __style.forestColor : __style.polarColor
    border.color: showBorder && !control.checked ? __style.mediumGreenColor : __style.transparentColor
  }
}
