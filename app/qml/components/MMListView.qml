/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls

// Drop-in replacement for ListView that also shows a scrollbar on desktop.
// Wraps a real ListView in a ScrollView internally so callers can keep using
// it exactly like a plain ListView, without knowing about the wrapping.

ScrollView {
  id: root

  property alias model: listView.model
  property alias delegate: listView.delegate
  property alias header: listView.header
  property alias footer: listView.footer
  property alias section: listView.section
  property alias add: listView.add
  property alias addDisplaced: listView.addDisplaced

  property alias orientation: listView.orientation
  property alias interactive: listView.interactive
  property alias maximumFlickVelocity: listView.maximumFlickVelocity
  property alias topMargin: listView.topMargin
  property alias bottomMargin: listView.bottomMargin

  property alias currentIndex: listView.currentIndex
  property alias count: listView.count

  property alias contentY: listView.contentY
  property alias atYEnd: listView.atYEnd

  function positionViewAtIndex( index, mode ) {
    listView.positionViewAtIndex( index, mode )
  }

  function positionViewAtEnd() {
    listView.positionViewAtEnd()
  }

  rightPadding: ScrollBar.vertical.visible ? ScrollBar.vertical.width * 2 : 0
  bottomPadding: ScrollBar.horizontal.visible ? ScrollBar.horizontal.height * 2 : 0

  ScrollBar.vertical.policy: !__inputUtils.isMobilePlatform() && listView.orientation === ListView.Vertical && listView.contentHeight > listView.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
  ScrollBar.horizontal.policy: !__inputUtils.isMobilePlatform() && listView.orientation === ListView.Horizontal && listView.contentWidth > listView.width ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff

  ListView {
    id: listView

    spacing: root.spacing

    // when flicking up really fast, we should go back to the first item
    onVerticalOvershootChanged: {
      if (verticalOvershoot < -200) {
        listView.contentY = -listView.topMargin
        listView.returnToBounds();
      }
    }
    delegateModelAccess: DelegateModel.ReadWrite
  }
}
