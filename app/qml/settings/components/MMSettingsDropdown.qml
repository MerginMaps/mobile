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

import "../../components" as MMComponents

MMSettingsItem {
  id: root

  property var model
  property int currentIndex: -1

  onClicked: {
    if(root.model?.count > 0) {
      drawerLoader.active = true
    }
  }

  Loader {
    id: drawerLoader

    active: false
    asynchronous: true
    sourceComponent: listComponent
  }

  Component {
    id: listComponent

    MMComponents.MMListDrawer {

      drawerHeader.title: root.title

      list.model: root.model

      list.delegate: MMComponents.MMListDelegate {
        text: model.text

        rightContent: MMComponents.MMIcon {
          source: __style.doneCircleIcon
          visible: index === root.currentIndex
        }

        onClicked: {
          root.currentIndex = index
          close()
        }
      }

      onClosed: drawerLoader.active = false

      Component.onCompleted: {
        open()
      }
    }
  }
}
