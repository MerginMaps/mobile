/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "./private"

/*
  MMToolbar is sticky toolbar on bottom of pages that contains few buttons.
  For buttons that cannot fit to screen, there are in more menu ("...")

  To use, add few instances of MMToolbarButton to model. Width and height of
  toolbar and buttons is calculated dynamically. If your model contains just
  one button it is always long button
*/
Rectangle {
  id: root

  /* required */ property var model // ObjectModel containing MMToolbarButton
  /* optional */ property int index: 0 // index of selected button for selectable toolbar

  onIndexChanged: toolbar.currentIndex = index
  onModelChanged: root.recalculate()

  height: __style.toolbarHeight + __style.safeAreaBottom
  width: window?.width ?? __style.safeAreaLeft + __style.safeAreaRight
  color: __style.forestColor

  Item {
    id: contentGroup

    height: __style.toolbarHeight
    width: parent.width - __style.safeAreaLeft - __style.safeAreaRight

    // center the content
    x: __style.safeAreaLeft

    ListView {
      id: toolbar

      onWidthChanged: root.recalculate()
      model: toolbarModel
      delegate: ( model.count === 1 ) && ( menuModel.count === 0 ) ?  longBtnComp : shortBtnComp
      anchors.fill: parent
      orientation: ListView.Horizontal
      interactive: false
      Component.onCompleted: root.recalculate()
    }
  }

  // buttons shown inside main toolbar
  ListModel {
    id: toolbarModel
  }

  // buttons that are shown in more menu (drawer)
  ListModel {
    id: menuModel
  }

  // More drawer menu
  MMListDrawer {
    id: menu

    drawerHeader.title: qsTr("More options")
    listModel: menuModel

    valueRole: "type"
    textRole: "text"
    imageRole: "iconSource"

    onClicked: function( btnData ) {
      menu.close()
      btnData.clicked()
    }
  }

  // Long button delegate
  Component {
    id: longBtnComp

    MMToolbarLongButton {
      width: ListView.view.width / ListView.view.model.count
      height: ListView.view.height

      enabled: model.btnData.enabled
      iconSource: model.btnData.iconSource
      iconColor: model.btnData.iconColor
      iconColorDisabled: model.btnData.iconColorDisabled
      text: model.btnData.text

      onClicked: model.btnData.clicked()
    }
  }

  // Short button delegate
  Component {
    id: shortBtnComp

    MMToolbarShortButton {
      width: ListView.view.width / ListView.view.model.count
      height: ListView.view.height

      selected: model.index === ListView.view.currentIndex

      rotating: model.btnData.iconRotateAnimationRunning
      enabled: model.btnData.enabled
      iconSourceSelected: model.btnData.iconSourceSelected
      iconSource: model.btnData.iconSource
      iconColor: model.btnData.iconColor
      iconColorDisabled: model.btnData.iconColorDisabled
      text: model.btnData.text

      onClicked: {
        root.index = model.index
        model.btnData.clicked()
      }
    }
  }

  // Button More '...'
  MMToolbarButton {
    id: moreButtonData

    text: qsTr("More")
    iconSource: __style.moreIcon
    onClicked: menu.open()
  }

  function recalculate() {
    toolbarModel.clear()
    menuModel.clear()

    // find how many visible buttons we need to place to toolbar or menu
    var visibleButtonsCount = 0
    for ( let i = 0; i < root.model.count; ++i ) {
      let btnData = root.model.get(i)
      if (btnData.visible) ++visibleButtonsCount
    }

    // find how many short buttons we can fit into toolbar and their width
    var maxButtonsInToolbar = Math.floor( toolbar.width / internal.minimumToolbarButtonWidth )
    var shouldHaveMoreButtonInToolbar = visibleButtonsCount > maxButtonsInToolbar
    var numberOfButtonsInToolbar = Math.min(maxButtonsInToolbar, visibleButtonsCount) // including "..." (more button)
    var numberOfButtonsInToolbarExcludingMoreButton = shouldHaveMoreButtonInToolbar ? numberOfButtonsInToolbar - 1 : numberOfButtonsInToolbar

    // fill models
    for ( var j = 0; j < root.model.count; ++j ) {
      var btnData = root.model.get(j)

      if (!btnData.visible) continue

      if (toolbarModel.count < numberOfButtonsInToolbarExcludingMoreButton) {
        // add to toolbar
        toolbarModel.append({ btnData: btnData })
      } else {
        // add to more menu
        menuModel.append({ iconSource: btnData.iconSource, text: btnData.text, type: btnData })
      }
    }

    // add more button if needed to main toolbar
    if (shouldHaveMoreButtonInToolbar)
    {
      toolbarModel.append({ btnData: moreButtonData })
    }

    // refrest selected button
    toolbar.currentIndex = root.index
  }

  QtObject {
    id: internal

    property double minimumToolbarButtonWidth: 100 * __dp
  }
}
