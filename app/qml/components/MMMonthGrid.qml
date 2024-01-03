// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick
import QtQuick.Templates as T

T.AbstractMonthGrid {
  id: control

  property date date: new Date()

  implicitWidth: Math.max(background ? background.implicitWidth : 0,
                          contentItem.implicitWidth + leftPadding + rightPadding)
  implicitHeight: Math.max(background ? background.implicitHeight : 0,
                           contentItem.implicitHeight + topPadding + bottomPadding)

  signal selectedDate(var date)

  delegate: Text {
    property bool highlighted: model.day === control.date.getDate()
                               && model.month === control.date.getMonth()
                               && model.year === control.date.getFullYear()

    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    opacity: model.month === control.month ? 1 : 0.2
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

    width: control.width

    Repeater {
      model: control.source
      delegate: control.delegate
    }
  }
}
