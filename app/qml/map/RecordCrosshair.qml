/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.14
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "../"

Item {
  id: root
  property point center: Qt.point(root.width / 2, root.height / 2)
  property real size: 100 * QgsQuick.Utils.dp

  Image {
    anchors.centerIn: parent
    height: root.size
    source: InputStyle.crosshairIcon
    sourceSize.height: height
    sourceSize.width: width
    width: height
  }
}
