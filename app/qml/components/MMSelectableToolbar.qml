/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

MMBaseToolbar {
  id: root

  signal clicked

  property alias model: buttonView.model
  property alias index: buttonView.currentIndex

  toolbarContent: GridView {
    id: buttonView

    anchors.fill: parent

    cellHeight: parent.height
    cellWidth: Math.floor(parent.width / root.model.count)

    highlightFollowsCurrentItem: false
    interactive: false
  }
}
