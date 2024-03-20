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

Tumbler {
  id: control

  delegate: Text {
    id: text

    text: modelData
    font: Math.abs(Tumbler.displacement) < 0.4 ? __style.t1 : __style.p4
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    opacity: 1.0 - Math.abs(Tumbler.displacement) / (control.visibleItemCount / 2)
    color: Math.abs(Tumbler.displacement) < 0.4 ? __style.forestColor : __style.nightColor
  }
}
