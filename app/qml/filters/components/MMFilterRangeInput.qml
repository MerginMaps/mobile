/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../../components"

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

  Row {
    id: rangeRow

    width: parent.width
    spacing: __style.margin12

    property bool rangeInvalid: {
      let fromVal = parseFloat( fromInput.text )
      let toVal = parseFloat( toInput.text )
      return !isNaN( fromVal ) && !isNaN( toVal ) && fromVal > toVal
    }

    MMFilterTextInput {
      id: fromInput

      width: ( parent.width - __style.margin12 ) / 2
      type: MMFilterTextInput.InputType.Number
      placeholderText: qsTr( "Min" )
      text: root.currentValue && root.currentValue[0] ? root.currentValue[0] : ""
      errorMsg: rangeRow.rangeInvalid ? qsTr( "\"Min\" must be less than \"Max\"" ) : ""

      onTextChanged: {
        debounceTimer.restart()
      }
    }

    MMFilterTextInput {
      id: toInput

      width: ( parent.width - __style.margin12 ) / 2
      type: MMFilterTextInput.InputType.Number
      placeholderText: qsTr( "Max" )
      text: root.currentValue && root.currentValue[1] ? root.currentValue[1] : ""
      errorMsg: rangeRow.rangeInvalid ? qsTr( "\"Min\" must be less than \"Max\"" ) : ""

      onTextChanged: {
        debounceTimer.restart()
      }
    }
  }

  Timer {
    id: debounceTimer
    interval: 300
    repeat: false
    onTriggered: {
      let newValues = []
      const valueFrom = parseFloat(fromInput.text)
      if ( !isNaN(valueFrom) ) {
        newValues[0] = valueFrom
      } else {
        newValues[0] = undefined
      }

      const valueTo = parseFloat(toInput.text)
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
