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
import "."

RoundButton {
  id: root

  implicitWidth: 40 * __dp
  implicitHeight: 40 * __dp
  hoverEnabled: root.enabled && Qt.styleHints.useHoverEffects

  property url iconSource: __style.backIcon
  property color iconColor: __style.forestColor
  property color borderColor: __style.polarColor
  property real borderWidth: 0

  property color bgndColor: __style.polarColor
  property color iconColorDisabled: __style.mediumGreenColor

  property color bgndHoverColor: __style.mediumGreenColor
  property color bgndColorDisabled: __style.transparentColor

  contentItem: MMIcon {
    anchors.centerIn: parent
    color: root.iconColor
    source: root.iconSource
  }
  states: State {
    name: "disabled"
    when: !root.enabled

    PropertyChanges {
      target: root
      bgndColor: root.bgndColorDisabled
    }

    PropertyChanges {
      target: root
      iconColor: root.iconColorDisabled
    }
  }

  background: Rectangle {
    color: root.down || root.hovered ? root.bgndHoverColor : root.bgndColor
    radius: root.implicitHeight / 2
    border.color: borderColor
    border.width: borderWidth
  }
}
