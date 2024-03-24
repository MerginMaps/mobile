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

MMDrawer {
  id: root

  property alias list: listViewComponent

  property alias emptyStateDelegate: emptyStateDelegateLoader.sourceComponent

  drawerContent: Item {
    width: parent.width
    height: listViewComponent.count === 0 ? emptyStateDelegateLoader.height : listViewComponent.height

    MMScrollView {
      width: parent.width
      height: Math.min( root.drawerContentAvailableHeight, contentHeight )

      enabled: contentHeight > height

      Loader {
        id: emptyStateDelegateLoader

        visible: listViewComponent.count === 0

        width: parent.width
      }
    }

    ListView {
      id: listViewComponent

      width: parent.width
      height: Math.min( root.drawerContentAvailableHeight, contentHeight )

      interactive: contentHeight > height

      clip: true
      maximumFlickVelocity: __androidUtils.isAndroid ? __style.scrollVelocityAndroid : maximumFlickVelocity
    }
  }
}
