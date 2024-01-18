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

T.AbstractDayOfWeekRow {
  id: root

  implicitWidth: Math.max(background ? background.implicitWidth : 0,
                          contentItem.implicitWidth + leftPadding + rightPadding)
  implicitHeight: Math.max(background ? background.implicitHeight : 0,
                           contentItem.implicitHeight + topPadding + bottomPadding)

  delegate: Text {
    required property string shortName

    text: shortName
    font: __style.t4
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    color: __style.mediumGreenColor
  }

  contentItem: Row {
    Repeater {
      model: root.source
      delegate: root.delegate
    }
  }
}
