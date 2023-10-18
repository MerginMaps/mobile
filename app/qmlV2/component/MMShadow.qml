/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import Qt5Compat.GraphicalEffects
import "../Style.js" as Style

DropShadow {
  id: shadow
  horizontalOffset: Style.shadowHorizontalOffset
  verticalOffset: Style.shadowVerticalOffset
  radius: Style.shadowRadius
  color: Style.shadowColor
  transparentBorder: true
}
