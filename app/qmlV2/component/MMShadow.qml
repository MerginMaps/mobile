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
import ".."


DropShadow {
  id: shadow
  horizontalOffset: 2 * __dp
  verticalOffset: 3 * __dp
  radius: 7 * __dp
  color: styleV2.shadowColor
  transparentBorder: true

  StyleV2 { id: styleV2 }
}
