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

//
// MMBadge - set text property
//

Label {
  font: __style.t4
  color: __style.polarColor

  leftPadding: __style.margin8
  rightPadding: __style.margin8

  lineHeightMode: Text.FixedHeight
  lineHeight: __style.fontLineHeight24

  verticalAlignment: Text.AlignVCenter
  horizontalAlignment: Text.AlignHCenter

  background: Rectangle {
    color: __style.forestColor
    radius: __style.radius40
  }
}
