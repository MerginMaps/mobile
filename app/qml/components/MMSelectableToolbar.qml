/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

Rectangle {
  id: control

  signal clicked

  property alias model: buttonView.model
  property alias index: buttonView.currentIndex

  height: __style.toolbarHeight
  color: __style.forestColor

  GridView {
    id: buttonView

    anchors.fill: parent

    cellHeight: __style.toolbarHeight
    cellWidth: Math.floor(control.width / control.model.count)

    highlightFollowsCurrentItem: false
  }
}
