/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQml

import "../../components"
import "./private"

Column {
  id: root

  width: parent.width
  spacing: __style.margin8

  required property string filterName
  required property var currentValue

  MMText {
    width: parent.width

    text: root.filterName

    font: __style.p6
    color: __style.nightColor
  }

  MMFilterBaseInput {
    id: filterInput

    width: root.width
    primaryPlaceholderText: qsTr( "Type to filter..." )
    primaryText: root.currentValue && root.currentValue[0] ? root.currentValue[0] : ""

    onPrimaryTextChanged: debounceTimer.restart()
  }

  Timer {
    id: debounceTimer
    interval: 300
    repeat: false
    onTriggered: {
      if (filterInput.text) {
        root.currentValue = [filterInput.text]
      } else {
        root.currentValue = undefined
      }
    }
  }
}
