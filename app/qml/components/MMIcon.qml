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
  id: root

  property alias source: icon.source
  property alias color: overlay.color
  property bool useCustomSize: false

  width: icon.implicitWidth
  height: icon.implicitHeight

  Image {
    id: icon

    source: __style.arrowLinkRightIcon
    anchors.fill: root.useCustomSize ? root : undefined
    anchors.centerIn: root.useCustomSize ? undefined : root
  }

  ColorOverlay {
    id: overlay

    anchors.fill: icon
    source: icon
  }
}
