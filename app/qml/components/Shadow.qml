/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.0
import QtGraphicalEffects 1.0
import "../"  // import InputStyle singleton

DropShadow {
  id: shadow
  color: "grey"
  horizontalOffset: 0
  radius: InputStyle.shadowRadius
  samples: InputStyle.shadowSamples
  transparentBorder: true
  verticalOffset: InputStyle.shadowVerticalOffset
}
