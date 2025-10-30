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
import QtQml

//
// Hot-fix for hotfix https://github.com/MerginMaps/mobile/issues/3417
// Seems like there is some issue with cache in ListView
//

ListView {
  id: root

  cacheBuffer: 0
  readonly property bool isMobile: (Qt.platform.os === "android" || Qt.platform.os === "ios")
  property int scrollSpace: !isMobile ? 10 : 0

  ScrollBar.vertical: ScrollBar {
    id: verticalScrollBar

    policy: ScrollBar.AlwaysOn
    visible: !isMobile && (root.contentHeight > root.height)
    opacity: (pressed || root.moving) ? 0.7 : 0.4

    contentItem: Rectangle {
      implicitWidth: 5
      radius: width / 2
      color: __style.darkGreenColor
    }
  }
}
