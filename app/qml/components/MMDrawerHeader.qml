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

//! Best to use MMDrawerHeader as the header component for the MMPage object

Rectangle {
  id: root

  property string title: ""
  property font titleFont: __style.t3

  property bool hasCloseButton: true
  property alias closeButton: closeBtn

  color: __style.transparentColor

  signal closeClicked

  implicitHeight: 60 * __dp
  implicitWidth: ApplicationWindow.window?.width ?? 0

  Text {
    // If the close button is visible, we need to properly center the text
    property real margin: internal.closeBtnRealWidth + internal.headerSpacing + __style.pageMargins

    anchors {
      fill: parent
      leftMargin: margin
      rightMargin: margin
    }

    text: root.title
    elide: Text.ElideMiddle

    font: root.titleFont
    color: __style.forestColor

    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
  }

  MMRoundButton {
    id: closeBtn

    anchors {
      right: parent.right
      rightMargin: __style.pageMargins
      verticalCenter: parent.verticalCenter
    }

    iconSource: __style.closeIcon
    bgndColor: __style.lightGreenColor

    visible: root.hasCloseButton
    onClicked: root.closeClicked()
  }

  QtObject {
    id: internal

    property real headerSpacing: 10 * __dp
    property real closeBtnRealWidth: closeBtn.visible ? closeBtn.width : 0
  }
}
