/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import Qt5Compat.GraphicalEffects
import "../Style.js" as Style

Rectangle {
  id: control

  signal deleteClicked
  signal editGeometryClicked
  signal saveClicked

  anchors {
    left: parent.left
    right: parent.right
    bottom: parent.bottom
  }
  height: Style.bottomBarHeight
  color: Style.forest

  enum Buttons {
    Delete, EditGeometry, Advanced, Save
  }

  enum States {
    First, Second, Third
  }

  required property var state

  ListModel {
    id: modelFirst

    ListElement { button: MMBottomBar.Buttons.Delete; priority: 2 }
    ListElement { button: MMBottomBar.Buttons.EditGeometry; priority: 2 }
    ListElement { button: MMBottomBar.Buttons.Advanced; priority: -1 }
  }

  ListModel {
    id: modelSecond

    ListElement { button: MMBottomBar.Buttons.Delete; priority: 0 }
    ListElement { button: MMBottomBar.Buttons.EditGeometry; priority: 0 }
  }

  ListModel {
    id: modelThird

    ListElement { button: MMBottomBar.Buttons.Delete; priority: 2 }
    ListElement { button: MMBottomBar.Buttons.EditGeometry; priority: 3 }
    ListElement { button: MMBottomBar.Buttons.Advanced; priority: -1 }
    ListElement { button: MMBottomBar.Buttons.Save; priority: 0 }
  }

  // buttons shown inside bottom bar
  ListModel {
    id: visibleButtonModel
  }

  // buttons that are not shown inside bottom bar, due to small space
  ListModel {
    id: invisibleButtonModel
  }

  ListView {
    id: buttonView

    anchors.fill: parent
    model: visibleButtonModel
    delegate: MMBottomBarButton {
      height: control.height
      onClicked: function(button) { buttonClicked(button) }
    }
    orientation: ListView.Horizontal
    leftMargin: Style.commonSpacing
    rightMargin: Style.commonSpacing
  }

  MMMenuDrawer {
    id: menu

    title: qsTr("More Options")
    onClicked: function(button) {
      menu.visible = false
      buttonClicked(button)
    }
  }

  onWidthChanged: setupBottomBar()

  // setup which buttons will be visible in bottom bar and which ones will be accessible by menu
  function setupBottomBar() {
    var currentFullModel
    switch (control.state) {
    case MMBottomBar.States.First: currentFullModel = modelFirst; break
    case MMBottomBar.States.Second: currentFullModel = modelSecond; break
    case MMBottomBar.States.Third: currentFullModel = modelThird; break
    }
    visibleButtonModel.clear()
    visibleButtonModel.modelReset()
    invisibleButtonModel.clear()
    invisibleButtonModel.modelReset()

    var showButtonMore = false
    var w = control.width - 2 * Style.commonSpacing

    for( var i = 0; i < currentFullModel.count; i++ ) {
      var entry = currentFullModel.get(i);
      if(entry.priority === -1) {
        showButtonMore = true
        continue
      }
      if(entry.priority === 0 || w > entry.priority * Style.minimumBottomBarButtonWidth ) {
        visibleButtonModel.append(entry)
      }
      else {
        invisibleButtonModel.append(entry)
      }
    }

    // if should be shown '...' button
    if(showButtonMore && invisibleButtonModel.count > 0) {
      visibleButtonModel.clear()
      visibleButtonModel.modelReset()
      invisibleButtonModel.clear()
      invisibleButtonModel.modelReset()
      for( i = 0; i < currentFullModel.count; i++ ) {
        entry = currentFullModel.get(i);
        if(entry.priority === 0 || entry.priority === -1 || w > (entry.priority+1) * Style.minimumBottomBarButtonWidth ) {
          visibleButtonModel.append(entry)
        }
        else {
          invisibleButtonModel.append(entry)
        }
      }
    }
  }

  function buttonClicked (button) {
    switch (button) {
    case MMBottomBar.Buttons.Delete: control.deleteClicked(); break
    case MMBottomBar.Buttons.EditGeometry: control.editGeometryClicked(); break
    case MMBottomBar.Buttons.Save: control.saveClicked(); break
    case MMBottomBar.Buttons.Advanced: {
      if (invisibleButtonModel.count > 0) {
        menu.model = invisibleButtonModel
        menu.visible = true
      }
      break
    }
    }
  }
}
