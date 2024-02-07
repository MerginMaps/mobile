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


//! Best to use MMHeader as the header component for the Page{} object or anchor it to the top of Drawer/Item

Item {
  id: root

  property string title: ""
  property font titleFont: __style.t3
  property color color: __style.transparentColor

  property bool personIconVisible: false
  property bool backVisible: true
  property alias backButton: backBtn

  //! When adding items on the right side of MMHeader (account icon, save button, slider, ...),
  //! make sure to set the width of the item via this property to keep the title centred to
  //! the page and elide properly
  property real rightMarginShift: 0

  signal backClicked
  signal personIconClicked

  implicitHeight: 60 * __dp
  implicitWidth: ApplicationWindow.window?.width ?? 0

  Rectangle {
    anchors.fill: parent
    color: root.color
  }

  Text {
    // If there is a right or a left icon, we need to shift the margin
    // of the opposite side to keep the text centred to the center of the screen
    property real leftMarginShift: {
      return Math.max( internal.backBtnRealWidth, root.rightMarginShift ) + internal.headerSpacing + internal.pageMargin
    }

    property real rightMarginShift: {
      return Math.max( internal.backBtnRealWidth, root.rightMarginShift ) + internal.headerSpacing + internal.pageMargin
    }

    anchors {
      fill: parent
      leftMargin: leftMarginShift
      rightMargin: rightMarginShift
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

    anchors {
      left: parent.left
      leftMargin: internal.pageMargin
      verticalCenter: parent.verticalCenter
    }

    visible: root.backVisible
    onClicked: root.backClicked()
  }

  Rectangle {
    id: personIconRect

    anchors {
      right: parent.right
      rightMargin: internal.pageMargin
      verticalCenter: parent.verticalCenter
    }

    width: 40 * __dp
    height: width
    radius: width / 2
    color: __style.fieldColor
    visible: root.personIconVisible

    MMIcon {
      anchors.centerIn: parent
      source: __style.personalIcon
      useCustomSize: true
      width: 24 * __dp
      height: 24 * __dp
    }

    MouseArea {
      anchors.fill: parent
      onClicked: root.personIconClicked()
    }
  }

  QtObject {
    id: internal

    property real headerSpacing: 10 * __dp
    property real pageMargin: 20 * __dp // TODO: move to mmstyle.h

    property real backBtnRealWidth: backBtn.visible ? backBtn.width : 0
  }
}
