/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
pragma ComponentBehavior: Bound

import QtQuick

import "../../components"
import "../../form/components" as MMFormComponents
import "./private"

Column {
  id: root

  width: parent.width
  spacing: __style.margin8

  required property string filterName
  required property var currentValue
  required property bool hasTime

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
    primaryPlaceholderText: qsTr( "From" )
    secondaryPlaceholderText: qsTr( "To" )
    errorMsg: rangeInvalid ? qsTr( "\"From\" must be sooner than \"To\"" ) : ""
    primaryText: {
      if ( !root.currentValue || !root.currentValue[0] ) return ""
      if ( root.hasTime ) return Qt.formatDateTime( root.currentValue[0] )
      return Qt.formatDate( root.currentValue[0] )
    }
    secondaryText: {
      if ( !root.currentValue || !root.currentValue[1] ) return ""
      if ( root.hasTime ) return Qt.formatDateTime( root.currentValue[1] )
      return Qt.formatDate( root.currentValue[1] )
    }

    onPrimaryTextClicked: {
      calendarLoader.callerIndex = 0
      calendarLoader.active = true
    }
    onSecondaryTextClicked: {
      calendarLoader.callerIndex = 1
      calendarLoader.active = true
    }

    onPrimaryRightContentClicked: (isChecked) => {
      if (isChecked) {
        if ( root.currentValue[1] ){
          root.currentValue = [undefined, root.currentValue[1]]
        } else {
          root.currentValue = undefined
        }
        root.currentValueChanged()
      } else {
        let currentTimestamp = new Date()

        if (root.hasTime) {
          filterInput.primaryText = Qt.formatDateTime(currentTimestamp)
        } else {
          filterInput.primaryText = Qt.formatDate(currentTimestamp)
        }

        if (!root.hasTime) {
          currentTimestamp.setHours(0, 0, 0, 0)
        }
        if (!root.currentValue) {
          root.currentValue = [currentTimestamp, undefined]
        } else {
          root.currentValue[0] = currentTimestamp
        }
        root.currentValueChanged()
      }
    }

    onSecondaryRightContentClicked: (isChecked) => {
      if (isChecked) {
        if ( root.currentValue[0] ){
          root.currentValue = [root.currentValue[0], undefined]
        } else {
          root.currentValue = undefined
        }
        root.currentValueChanged()

      } else {
        let currentTimestamp = new Date()

        if (root.hasTime) {
          filterInput.secondaryText = Qt.formatDateTime(currentTimestamp)
        } else {
          filterInput.secondaryText = Qt.formatDate(currentTimestamp)
        }

        if (!root.hasTime) {
          currentTimestamp.setHours(0, 0, 0, 0)
        }
        if (!root.currentValue) {
          root.currentValue = [undefined, currentTimestamp]
        } else {
          root.currentValue[1] = currentTimestamp
          root.currentValueChanged()
        }
      }
    }

    property bool rangeInvalid: {
      if ( !root.currentValue || !root.currentValue[0] || !root.currentValue[1] ){
        return false
      }
      return root.currentValue[0] > root.currentValue[1]
    }
  }

  Loader {
    id: calendarLoader

    property int callerIndex
    active: false
    sourceComponent: calendarComponent
  }

  Component {
    id: calendarComponent

    MMFormComponents.MMCalendarDrawer {
      hasDatePicker: true
      hasTimePicker: root.hasTime
      dateTime: root.currentValue && root.currentValue[calendarLoader.callerIndex] ? root.currentValue[calendarLoader.callerIndex] : new Date()

      onPrimaryButtonClicked: {
        let currentTimestamp = dateTime
        if (!root.hasTime) {
          currentTimestamp.setHours(0, 0, 0, 0)
        }
        if (!root.currentValue){
          root.currentValue = calendarLoader.callerIndex ? [undefined, currentTimestamp] : [currentTimestamp, undefined]
        } else {
          root.currentValue[calendarLoader.callerIndex] = currentTimestamp
          root.currentValueChanged()
        }

        if ( calendarLoader.callerIndex ){
          filterInput.secondaryText = root.hasTime ? Qt.formatDateTime(dateTime) : Qt.formatDate(dateTime)
        } else {
          filterInput.primaryText = root.hasTime ? Qt.formatDateTime(dateTime) : Qt.formatDate(dateTime)
        }
      }
      onClosed: calendarLoader.active = false
      Component.onCompleted: open()
    }
  }
}
