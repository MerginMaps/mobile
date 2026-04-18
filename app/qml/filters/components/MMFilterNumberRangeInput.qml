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

    doubleInput: true
    primaryPlaceholderText: qsTr( "Min" )
    secondaryPlaceholderText: qsTr( "Max" )
    primaryText: root.currentValue && root.currentValue[0] ? root.currentValue[0] : ""
    secondaryText: root.currentValue && root.currentValue[1] ? root.currentValue[1] : ""
    errorMsg: rangeRow.rangeInvalid ? qsTr( "\"Min\" must be less than \"Max\"" ) : ""

    onPrimaryTextChanged: {
      debounceTimer.restart()
    }

    onSecondaryTextChanged: {
      debounceTimer.restart()
    }

    property bool rangeInvalid: {
      let fromVal = parseFloat( filterInput.primaryText )
      let toVal = parseFloat( filterInput.secondaryText )
      return !isNaN( fromVal ) && !isNaN( toVal ) && fromVal > toVal
    }
  }

  Timer {
    id: debounceTimer
    interval: 300
    repeat: false
    onTriggered: {
      let newValues = []
      const valueFrom = parseFloat( filterInput.primaryText )
      if ( !isNaN(valueFrom) ) {
        newValues[0] = valueFrom
      } else {
        newValues[0] = undefined
      }

      const valueTo = parseFloat( filterInput.secondaryText )
      if ( !isNaN(valueTo) ) {
        newValues[1] = valueTo
      } else {
        newValues[1] = undefined
      }

      if ( newValues[0] === undefined && newValues[1] === undefined ) {
        root.currentValue = undefined
      } else {
        root.currentValue = newValues
      }
    }
  }
}
