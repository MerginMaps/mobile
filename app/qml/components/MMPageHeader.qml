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

  property alias leftItemContent: leftButtonGroup.children
  property alias rightItemContent: rightButtonGroup.children

  property real topSpacing: __style.safeAreaTop // offset size from top of the page, normally safeAreaTop, but can be overriden (e.g. login page)
  property real baseHeaderHeight: __style.row60

  color: __style.transparentColor

  signal backClicked

  implicitHeight: root.baseHeaderHeight + root.topSpacing
  implicitWidth: ApplicationWindow.window?.width ?? 0

  Text {
    // If there is a right or a left icon, we need to shift the margin
    // of the opposite side to keep the text centred to the center of the screen
    property real leftMarginShift: {
      return Math.max( internal.backBtnRealWidth, internal.leftGroupRealWidth, rightButtonGroup.width ) + internal.headerSpacing + __style.pageMargins
    }

    property real rightMarginShift: {
      return Math.max( internal.backBtnRealWidth, internal.leftGroupRealWidth, rightButtonGroup.width ) + internal.headerSpacing + __style.pageMargins
    }

    anchors {
      fill: parent
      leftMargin: leftMarginShift
      rightMargin: rightMarginShift
      topMargin: root.topSpacing
    }

    text: root.title?.toString()?.replace(/\n/g, ' ') ?? ''
    elide: Text.ElideMiddle
    textFormat: Text.PlainText

    font: root.titleFont
    color: __style.forestColor

    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
  }

  MMRoundButton {
    id: backBtn

    x: __style.pageMargins + __style.safeAreaLeft
    y: ( root.baseHeaderHeight / 2 - height / 2 ) + root.topSpacing

    visible: root.backVisible
    onClicked: root.backClicked()
  }

  Item {
    id: leftButtonGroup

    x: __style.pageMargins + __style.safeAreaLeft
    y: ( root.baseHeaderHeight / 2 - height / 2 ) + root.topSpacing

    width: childrenRect.width
    height: childrenRect.height

    visible: !root.backVisible // Only show when back button is hidden
  }

  Item {
    id: rightButtonGroup

    x: parent.width - __style.pageMargins - __style.safeAreaRight - width
    y: ( root.baseHeaderHeight / 2 - height / 2 ) + root.topSpacing

    width: childrenRect.width
    height: parent.height
  }

  QtObject {
    id: internal

    property real headerSpacing: 10 * __dp
    property real backBtnRealWidth: backBtn.visible ? backBtn.width : 0
    property real leftGroupRealWidth: leftButtonGroup.visible ? leftButtonGroup.width : 0
  }
}
