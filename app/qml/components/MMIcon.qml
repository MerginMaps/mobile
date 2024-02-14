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

// Recommendation: use the size property instead of width and height

Item {
  id: root

  required property url source
  property real size: __style.icon24
  property color color: __style.forestColor

  width: size
  height: size

  Image {
    id: icon

    source: root.source
    anchors.fill: parent
  }

  ColorOverlay {
    id: overlay

    color: root.color
    anchors.fill: icon
    source: icon
  }
}
