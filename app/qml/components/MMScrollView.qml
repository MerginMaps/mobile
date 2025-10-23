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

  contentWidth: availableWidth // to only scroll vertically

  ScrollBar.vertical.policy: ScrollBar.AlwaysOff
  ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

}
