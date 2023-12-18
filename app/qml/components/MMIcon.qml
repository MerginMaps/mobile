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

Item {
  id: control

  property alias source: icon.source
  property alias color: overlay.color

  width: icon.implicitWidth
  height: icon.implicitHeight

  Image {
    id: icon

    source: __style.arrowLinkRightIcon
    anchors.centerIn: control
  }

  ColorOverlay {
    id: overlay

    anchors.fill: icon
    source: icon
  }
}
