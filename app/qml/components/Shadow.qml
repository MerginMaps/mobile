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
import ".."  // import InputStyle singleton

DropShadow {
    id: shadow
    horizontalOffset: 0
    verticalOffset: InputStyle.shadowVerticalOffset
    radius: InputStyle.shadowRadius
    color: "grey"
    transparentBorder: true
}
