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

// Drop-in replacement for ListView that also shows a scrollbar on desktop.
// Rooted in a plain Item rather than ScrollView, so only the properties
// aliased below are exposed - ScrollView/Pane/Control have their own
// contentWidth/spacing/padding that would otherwise shadow ListView's.

Item {
  id: root

  property alias model: listView.model
  property alias delegate: listView.delegate
  property alias header: listView.header
  property alias footer: listView.footer
  property alias section: listView.section
  property alias add: listView.add
  property alias addDisplaced: listView.addDisplaced

  property alias orientation: listView.orientation
  property alias spacing: listView.spacing
  property alias interactive: listView.interactive
  property alias maximumFlickVelocity: listView.maximumFlickVelocity
  property alias topMargin: listView.topMargin
  property alias bottomMargin: listView.bottomMargin

  property alias currentIndex: listView.currentIndex
  property alias count: listView.count

  property alias contentY: listView.contentY
  property alias contentWidth: listView.contentWidth
  property alias contentHeight: listView.contentHeight
  property alias atYEnd: listView.atYEnd

  implicitWidth: scrollView.implicitWidth
  implicitHeight: scrollView.implicitHeight

  function positionViewAtIndex( index, mode ) {
    listView.positionViewAtIndex( index, mode )
  }

  function positionViewAtEnd() {
    listView.positionViewAtEnd()
  }

  ScrollView {
    id: scrollView

    anchors.fill: parent

    // reserve room for the scrollbar so content isn't drawn under it
    rightPadding: ScrollBar.vertical.visible ? ScrollBar.vertical.width * 2 : 0
    bottomPadding: ScrollBar.horizontal.visible ? ScrollBar.horizontal.height * 2 : 0

    // non-interactive lists can't be scrolled, so never show a scrollbar for them
    ScrollBar.vertical.policy: listView.interactive && !__inputUtils.isMobilePlatform() && listView.orientation === ListView.Vertical && listView.contentHeight > listView.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
    ScrollBar.horizontal.policy: listView.interactive && !__inputUtils.isMobilePlatform() && listView.orientation === ListView.Horizontal && listView.contentWidth > listView.width ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff

    ListView {
      id: listView

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
}
