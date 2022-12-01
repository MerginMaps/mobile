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

import ".."

Item {
  id: root

  signal searchTextChanged( string searchText )

  // set width manually
  height: InputStyle.rowHeightMedium

  Rectangle {
    id: content

    anchors.fill: parent

    radius: InputStyle.cornerRadius
    color: InputStyle.panelBackgroundLight

    Row {
      id: contentrow

      anchors {
        left: parent.left
        leftMargin: InputStyle.searchBoxV2Spacing
        right: parent.right
        rightMargin: InputStyle.searchBoxV2Spacing
        verticalCenter: parent.verticalCenter
      }

      height: childrenRect.height

      Image {
        id: searchIcon

        source: InputStyle.searchIconV2
        height: content.height * 0.65
        width: height
      }

      TextField {
        id: searchfield

        height: content.height * 0.65
        width: contentrow.width - searchIcon.width

        font.italic: true
        placeholderText: qsTr("Search")

        onTextChanged: searchTextChanged( text )

        background: Rectangle {
          color: "transparent"
        }
      }
    }
  }

  function setActive() {
    searchfield.forceActiveFocus()
  }
}
