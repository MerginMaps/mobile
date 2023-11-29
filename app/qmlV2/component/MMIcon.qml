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

Item {
  id: control

  property alias source: icon.source
  property alias color: color.color

  width: icon.implicitWidth
  height: icon.implicitHeight

  StyleV2 { id: styleV2 }

  Image {
    id: icon

    source: styleV2.arrowLinkRightIcon
    anchors.centerIn: control
  }

  ColorOverlay {
    id: color

    anchors.fill: icon
    source: icon
  }
}
