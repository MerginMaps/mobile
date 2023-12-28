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

Rectangle {
  id: control

  signal clicked

  required property var model

  readonly property double minimumToolbarButtonWidth: 100 * __dp

  anchors {
    left: parent.left
    right: parent.right
    bottom: parent.bottom
  }
  height: __style.toolbarHeight
  color: __style.forestColor

  onWidthChanged: setupBottomBar()

  // buttons shown inside toolbar
  ObjectModel {
    id: visibleButtonModel
  }

  // buttons that are not shown inside toolbar, due to small space
  ObjectModel {
    id: invisibleButtonModel
  }

  GridView {
    id: buttonView

    model: visibleButtonModel
    anchors.fill: parent
    cellHeight: __style.toolbarHeight
    interactive: false
  }

  MMMenuDrawer {
    id: menu

    title: qsTr("More options")
    model: invisibleButtonModel
    onClicked: function(button) {
      menu.visible = false
      buttonClicked(button)
    }
  }

  // Button More '...'
  Component {
    id: componentMore
    MMToolbarButton {
      text: qsTr("More")
      iconSource: __style.moreIcon
      onClicked: menu.visible = true
    }
  }
  Loader { id: buttonMore; sourceComponent: componentMore; visible: false }

  function setupBottomBar() {
    var m = control.model
    var c = m.count
    var w = control.width
    var button

    // add all buttons (max 4) into toolbar
    visibleButtonModel.clear()
    if(c <= 4 || w >= c*control.minimumToolbarButtonWidth) {
      for( var i = 0; i < c; i++ ) {
        button = m.get(i)
        if(button.isMenuButton !== undefined)
          button.isMenuButton = false
        button.width = Math.floor(w / c)
        visibleButtonModel.append(button)
      }
      buttonView.cellWidth = Math.floor(w / c)
    }
    else {
      // not all buttons are visible in toolbar due to width
      // the past of them will apper in the menu inside '...' button
      var maxVisible = Math.floor(w/control.minimumToolbarButtonWidth)
      if(maxVisible<4)
        maxVisible = 4
      for( i = 0; i < maxVisible-1; i++ ) {
        if(maxVisible===4 || w >= i*control.minimumToolbarButtonWidth) {
          button = m.get(i)
          button.isMenuButton = false
          button.width = Math.floor(w / maxVisible)
          visibleButtonModel.append(button)
        }
      }
      // add More '...' button
      button = buttonMore
      button.visible = true
      button.width = maxVisible ? w / maxVisible : w
      visibleButtonModel.append( button )
      buttonView.cellWidth = Math.floor(maxVisible ? w / maxVisible : w)

      // add all other buttons inside the '...' button
      invisibleButtonModel.clear()
      for( i = maxVisible-1; i < c; i++ ) {
        if(i<0)
          continue
        button = m.get(i)
        button.isMenuButton = true
        button.width = Math.floor(w)
        invisibleButtonModel.append(button)
      }
    }
  }
}
