/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

/**
  Base toolbar - take care of safe margins and width and height.
  Do not use directly in-app, see MMSelectableToolbar and MMToolbar
*/
Rectangle {
  id: root

  property alias toolbarContent: contentGroup.children

  height: __style.toolbarHeight + __style.safeAreaBottom
  width: window?.width ?? __style.safeAreaLeft + __style.safeAreaRight
  color: __style.forestColor

  Item {
    id: contentGroup

    height: __style.toolbarHeight
    width: parent.width - __style.safeAreaLeft - __style.safeAreaRight

    // center the content
    x: __style.safeAreaLeft
  }

}
