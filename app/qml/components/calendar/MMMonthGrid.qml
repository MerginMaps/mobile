/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Templates as T

T.AbstractMonthGrid {
  id: root

  property date date: new Date()

  implicitWidth: Math.max(background ? background.implicitWidth : 0,
                          contentItem.implicitWidth + leftPadding + rightPadding)
  implicitHeight: Math.max(background ? background.implicitHeight : 0,
                           contentItem.implicitHeight + topPadding + bottomPadding)

  signal selectedDate(var date)

  delegate: Text {
    property bool highlighted: model.day === root.date.getDate()
                               && model.month === root.date.getMonth()
                               && model.year === root.date.getFullYear()

    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    opacity: model.month === root.month ? 1 : 0.2
    text: model.day
    font: highlighted ? __style.h3 : __style.p2
    color: highlighted ? __style.forestColor : __style.nightColor

    MouseArea {
      anchors.fill: parent
      onClicked: selectedDate(new Date( model.year, model.month, model.day))
    }

    Rectangle {
      anchors.centerIn: parent
      width: 46 * __dp
      height: width
      radius: width / 2
      color: __style.lightGreenColor
      visible: parent.highlighted
      z: -1
    }
  }

  contentItem: Grid {
    rows: 6
    columns: 7
    rowSpacing: 15 * __dp

    width: root.width

    Repeater {
      model: root.source
      delegate: root.delegate
    }
  }
}
