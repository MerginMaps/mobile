/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

Item {
  id: root

  property real bw: 2

  Rectangle {
    color: "red"
    anchors.fill: parent
  }

  /*
  GalleryGridItem {
    bw: root.bw

    xMin: 0
    yMin: 0
    xMax: parent.width
    yMax: parent.height

    left: __style.safeAreaLeft
    right: __style.safeAreaRight
    top: __style.safeAreaTop
    bottom: __style.safeAreaBottom
    name: "safearea"
    color: "lightblue"
  }
  */
}
