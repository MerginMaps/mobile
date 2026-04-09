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

  required property string fieldDisplayName
  required property var currentValue
  required property var currentValueTo
  required property string fieldLayerId
  required property string fieldName

  property string initialFrom: {
    let v = root.currentValue
    return ( v !== null && v !== undefined ) ? String( v ) : ""
  }
  property string initialTo: {
    let v = root.currentValueTo
    return ( v !== null && v !== undefined ) ? String( v ) : ""
  }

  property bool _initialized: false
  Component.onCompleted: _initialized = true

  MMText {
    width: parent.width
    text: root.fieldDisplayName
    font: __style.p6
    color: __style.nightColor
    visible: root.fieldDisplayName !== ""
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
      text: root.initialFrom
      errorMsg: rangeRow.rangeInvalid ? qsTr( "\"Min\" must be less than \"Max\"" ) : ""

      onTextChanged: {
        if ( !root._initialized ) return
        debounceTimer.restart()
      }
    }

    MMFilterTextInput {
      id: toInput

      width: ( parent.width - __style.margin12 ) / 2
      type: MMFilterTextInput.InputType.Number
      placeholderText: qsTr( "Max" )
      text: root.initialTo
      errorMsg: rangeRow.rangeInvalid ? qsTr( "\"Min\" must be less than \"Max\"" ) : ""

      onTextChanged: {
        if ( !root._initialized ) return
        debounceTimer.restart()
      }
    }
  }

  Timer {
    id: debounceTimer
    interval: 300
    repeat: false
    onTriggered: {
      if ( root.fieldLayerId && root.fieldName )
        __activeProject.filterController.setNumberFilter( root.fieldLayerId, root.fieldName, fromInput.text, toInput.text )
    }
  }
}
