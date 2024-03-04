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
import QtQuick.Layouts

import "."

//! Best to use MMPageHeader as the header component for the MMPage object

Rectangle {
  id: root

  property string title: ""
  property font titleFont: __style.t3

  property bool backVisible: true
  property alias backButton: backBtn

  color: __style.transparentColor

  //! When adding items on the right side of MMPageHeader (account icon, save button, slider, ...),
  //! make sure to set the width of the item via this property to keep the title centred to
  //! the page and elide properly
  property real rightMarginShift: 0

  signal backClicked

  implicitHeight: internal.baseHeaderHeight + __style.safeAreaTop
  implicitWidth: ApplicationWindow.window?.width ?? 0

  Text {
    // If there is a right or a left icon, we need to shift the margin
    // of the opposite side to keep the text centred to the center of the screen
    property real leftMarginShift: {
      return Math.max( internal.backBtnRealWidth, root.rightMarginShift ) + internal.headerSpacing + __style.pageMargins
    }

    property real rightMarginShift: {
      return Math.max( internal.backBtnRealWidth, root.rightMarginShift ) + internal.headerSpacing + __style.pageMargins
    }

    anchors {
      fill: parent
      leftMargin: leftMarginShift
      rightMargin: rightMarginShift
      topMargin: __style.safeAreaTop
    }

    text: root.title
    elide: Text.ElideMiddle

    font: root.titleFont
    color: __style.forestColor

    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
  }

  MMRoundButton {
    id: backBtn

    x: __style.pageMargins
    y: ( internal.baseHeaderHeight / 2 - height / 2 ) + __style.safeAreaTop

    visible: root.backVisible
    onClicked: root.backClicked()
  }

  QtObject {
    id: internal

    property real headerSpacing: 10 * __dp
    property real baseHeaderHeight: __style.row60
    property real backBtnRealWidth: backBtn.visible ? backBtn.width : 0
  }
}
