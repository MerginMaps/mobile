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

  required property var model
  readonly property double minimumToolbarButtonWidth: 100 * __dp
  property int maxButtonsInToolbar: 4

  signal clicked

  onModelChanged: root.setupBottomBar()

  toolbarContent: GridView {
    id: buttonView

    onWidthChanged: root.setupBottomBar()
    model: visibleButtonModel
    anchors.fill: parent
    cellHeight: parent.height
    interactive: false
  }

  // buttons shown inside toolbar
  ObjectModel {
    id: visibleButtonModel
  }

  // buttons that are not shown inside toolbar, due to small space
  ObjectModel {
    id: invisibleButtonModel
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
    var buttonModel = root.model
    var buttonWidth = buttonView.width
    var button

    let filteredbuttons = []

    for ( var j = 0; j < buttonModel.count; j++ ) {
      if ( buttonModel.get(j) && buttonModel.get(j).visibilityMode === true )
        filteredbuttons.push( buttonModel.get(j) )
    }

    var buttonsCount = filteredbuttons.length

    // add all buttons (max maxButtonsInToolbar) into toolbar
    visibleButtonModel.clear()
    if(buttonsCount <= maxButtonsInToolbar || buttonWidth >= buttonsCount*root.minimumToolbarButtonWidth) {
      for( var i = 0; i < buttonsCount; i++ ) {
        button = filteredbuttons[i]
        if(button.isMenuButton !== undefined)
          button.isMenuButton = false
        button.width = Math.floor(buttonWidth / buttonsCount)
        visibleButtonModel.append(button)
      }
      buttonView.cellWidth = Math.floor(buttonWidth / buttonsCount)
    }
    else {
      // not all buttons are visible in toolbar due to width
      // the past of them will apper in the menu inside '...' button
      var maxVisible = Math.floor(buttonWidth/root.minimumToolbarButtonWidth)
      if(maxVisible<maxButtonsInToolbar)
        maxVisible = maxButtonsInToolbar
      for( i = 0; i < maxVisible-1; i++ ) {
        if(maxVisible===maxButtonsInToolbar || buttonWidth >= i*root.minimumToolbarButtonWidth) {
          button = filteredbuttons[i]
          button.isMenuButton = false
          button.width = Math.floor(buttonWidth / maxVisible)
          visibleButtonModel.append(button)
        }
      }
      // add More '...' button
      button = buttonMore
      button.visible = true
      button.width = maxVisible ? buttonWidth / maxVisible : buttonWidth
      visibleButtonModel.append( button )
      buttonView.cellWidth = Math.floor(maxVisible ? buttonWidth / maxVisible : buttonWidth)

      // add all other buttons inside the '...' button
      invisibleButtonModel.clear()
      for( i = maxVisible-1; i < buttonsCount; i++ ) {
        if(i<0)
          continue
        button = filteredbuttons[i]
        button.isMenuButton = true
        button.width = Math.floor(buttonWidth)
        button.parentMenu = menu
        invisibleButtonModel.append(button)
      }
    }
  }

  function closeMenu() {
    menu.close()
  }
}
