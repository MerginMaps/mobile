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

  property bool showFullScreen: false
  property alias emptyStateDelegate: emptyStateDelegateLoader.sourceComponent

  interactive: false

  drawerContent: Item {
    width: parent.width
    height: {
      if ( root.showFullScreen ) return root.drawerContentAvailableHeight
      if ( listViewComponent.count === 0 ) return emptyStateDelegateLoader.height
      return listViewComponent.height
    }

    MMScrollView {
      width: parent.width
      height: Math.min( root.drawerContentAvailableHeight, contentHeight )

      enabled: emptyStateDelegateLoader.visible

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
