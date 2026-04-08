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
  required property string fieldLayerId
  required property string fieldName
  required property var currentValue

  property string initialValue: {
    let v = root.currentValue
    return ( v !== null && v !== undefined && v !== "" ) ? String( v ) : ""
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

  MMFilterTextInput {
    id: filterInput

    width: parent.width
    type: MMFilterTextInput.InputType.Text
    placeholderText: qsTr( "Type to filter..." )
    text: root.initialValue

    onTextChanged: {
      if ( !root._initialized ) return
      debounceTimer.restart()
    }
  }

  Timer {
    id: debounceTimer
    interval: 300
    repeat: false
    onTriggered: {
      __activeProject.filterController.setTextFilter( root.fieldLayerId, root.fieldName, filterInput.text )
    }
  }
}
