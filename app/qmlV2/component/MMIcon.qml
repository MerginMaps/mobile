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

Item {
  id: control

  property alias source: icon.source
  property alias color: color.color

  width: icon.implicitWidth
  height: icon.implicitHeight

  Image {
    id: icon
    source: Style.arrowLinkRight
    anchors.centerIn: control
  }

  ColorOverlay {
    id: color

    anchors.fill: icon
    source: icon
  }
}
