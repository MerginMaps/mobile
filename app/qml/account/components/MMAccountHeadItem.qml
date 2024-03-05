/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

Rectangle {
  id: root

  property string abbrv // max 2 letters
  property bool hasNotification: false

  property alias text: txt

  implicitWidth: 80 * __dp
  implicitHeight: 80 * __dp

  radius: implicitWidth

  color: __style.fieldColor

  Text {
    id: txt

    anchors.centerIn: parent

    text: root.abbrv

    font: __style.h3
    color: __style.forestColor
  }

  Rectangle {

    width: parent.width / 4
    height: parent.height / 4

    border.width: 2 * __dp
    border.color: __style.lightGreenColor

    radius: width
    color: __style.negativeColor

    anchors {
      right: parent.right
      rightMargin: __style.margin2
      top: parent.top
      topMargin: __style.margin2
    }

    visible: root.hasNotification
  }
}
