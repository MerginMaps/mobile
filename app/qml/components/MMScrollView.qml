/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls

// Convenient class to use as a pageContent or drawerContent
// base element to make the content scroll

ScrollView {
  id: root

  rightPadding: ScrollBar.vertical.visible ? ScrollBar.vertical.width * 2 : 0

  contentWidth: availableWidth // to only scroll vertically

  ScrollBar.vertical.policy: !__inputUtils.isMobilePlatform() && root.contentHeight > root.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
  ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
}
