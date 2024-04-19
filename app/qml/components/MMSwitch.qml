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

Switch {
  id: root

  property color checkedBgColor: enabled ? __style.grassColor : __style.mediumGreenColor
  property color uncheckedBgColor: __style.polarColor
  property color handleColor: enabled ? __style.forestColor : __style.mediumGreyColor

  topPadding: 0
  rightPadding: 0
  bottomPadding: 0
  leftPadding: 0

  contentItem: Text {
    text: root.text
    font: __style.p5
    color: root.handleColor
    verticalAlignment: Text.AlignVCenter
    leftPadding: root.indicator.width + ( text ? root.spacing : 0 )
  }

  indicator: Rectangle {
    id: indicatorRectangle

    implicitWidth: 48 * __dp
    implicitHeight: 28 * __dp
    x: root.leftPadding
    radius: implicitHeight / 2
    color: root.checked ? root.checkedBgColor : root.uncheckedBgColor

    Rectangle {
      x: root.checked ? parent.width - width - radius/2 : radius/2
      width: 20 * __dp
      height: width
      radius: width / 2
      color: root.handleColor
      anchors.verticalCenter: parent.verticalCenter
    }
  }
}
