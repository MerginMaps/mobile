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

Item {
  id: root

  property alias title: titletxt.text
  property alias value: valuetxt.text

  property bool alignmentRight: false

  implicitHeight: contentColumn.implicitHeight

  Rectangle {
    anchors.fill: parent
    color: __style.polarColor
  }

  Column {
    id: contentColumn

    topPadding: __style.margin8
    bottomPadding: __style.margin8

    width: parent.width

    spacing: 0

    MMComponents.MMText {
      id: titletxt

      leftPadding: alignmentRight ? __style.margin4 : 0
      rightPadding: alignmentRight ? 0 : __style.margin4

      width: parent.width - leftPadding - rightPadding
      x: leftPadding

      font: __style.p6

      horizontalAlignment: alignmentRight ? Text.AlignRight : Text.AlignLeft
    }

    MMComponents.MMText {
      id: valuetxt

      leftPadding: alignmentRight ? __style.margin4 : 0
      rightPadding: alignmentRight ? 0 : __style.margin4

      width: parent.width - leftPadding - rightPadding
      x: leftPadding

      font: __style.t3
      color: __style.nightColor

      horizontalAlignment: alignmentRight ? Text.AlignRight : Text.AlignLeft
    }
  }
}
