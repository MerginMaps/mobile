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
import "../../form/components" as MMFormComponents

Column {
  id: root

  width: parent.width
  spacing: __style.margin8

  required property string fieldDisplayName
  required property bool hasTime
  required property var currentValue
  required property var currentValueTo
  required property string fieldLayerId
  required property string fieldName

  property var initialFromDate: {
    let v = root.currentValue
    if ( v === null || v === undefined ) return null
    let d = new Date( v )
    return isNaN( d.getTime() ) ? null : d
  }
  property var initialToDate: {
    let v = root.currentValueTo
    if ( v === null || v === undefined ) return null
    let d = new Date( v )
    return isNaN( d.getTime() ) ? null : d
  }

  property var fromDate: initialFromDate
  property var toDate: initialToDate

  property bool rangeInvalid: fromDate !== null && toDate !== null && fromDate.getTime() > toDate.getTime()

  property bool _initialized: false
  Component.onCompleted: _initialized = true

  function applyDateFilter() {
    if ( !_initialized || !fieldLayerId || !fieldName ) return
    __activeProject.filterController.setDateFilter( fieldLayerId, fieldName, fromDate, toDate, hasTime )
  }

  onFromDateChanged: applyDateFilter()
  onToDateChanged: applyDateFilter()

  MMText {
    width: parent.width
    text: root.fieldDisplayName
    font: __style.p6
    color: __style.nightColor
    visible: root.fieldDisplayName !== ""
  }

  Row {
    width: parent.width
    spacing: __style.margin12

    Item {
      width: ( parent.width - __style.margin12 ) / 2
      height: fromDateInput.height

      MMFilterTextInput {
        id: fromDateInput

        width: parent.width
        type: MMFilterTextInput.InputType.Date
        checked: root.fromDate !== null && !root.rangeInvalid
        placeholderText: qsTr( "From" )
        errorMsg: root.rangeInvalid ? qsTr( "\"From\" must be less than \"To\"" ) : ""
        text: {
          if ( !root.fromDate ) return ""
          if ( root.hasTime ) return Qt.formatDateTime( root.fromDate, Qt.DefaultLocaleShortDate )
          return Qt.formatDate( root.fromDate, Qt.DefaultLocaleShortDate )
        }

        onTextClicked: fromCalendarLoader.active = true
        onRightContentClicked: {
          if ( root.fromDate ) {
            root.fromDate = null
          } else {
            fromCalendarLoader.active = true
          }
        }
      }

      Loader {
        id: fromCalendarLoader
        active: false
        sourceComponent: fromCalendarComponent
      }

      Component {
        id: fromCalendarComponent

        MMFormComponents.MMCalendarDrawer {
          hasDatePicker: true
          hasTimePicker: root.hasTime
          dateTime: root.fromDate ? root.fromDate : new Date()

          onPrimaryButtonClicked: {
            root.fromDate = dateTime
          }
          onClosed: fromCalendarLoader.active = false
          Component.onCompleted: open()
        }
      }
    }

    Item {
      width: ( parent.width - __style.margin12 ) / 2
      height: toDateInput.height

      MMFilterTextInput {
        id: toDateInput

        width: parent.width
        type: MMFilterTextInput.InputType.Date
        checked: root.toDate !== null && !root.rangeInvalid
        placeholderText: qsTr( "To" )
        errorMsg: root.rangeInvalid ? qsTr( "\"To\" must be greater than \"From\"" ) : ""
        text: {
          if ( !root.toDate ) return ""
          if ( root.hasTime ) return Qt.formatDateTime( root.toDate, Qt.DefaultLocaleShortDate )
          return Qt.formatDate( root.toDate, Qt.DefaultLocaleShortDate )
        }

        onTextClicked: toCalendarLoader.active = true
        onRightContentClicked: {
          if ( root.toDate ) {
            root.toDate = null
          } else {
            toCalendarLoader.active = true
          }
        }
      }

      Loader {
        id: toCalendarLoader
        active: false
        sourceComponent: toCalendarComponent
      }

      Component {
        id: toCalendarComponent

        MMFormComponents.MMCalendarDrawer {
          hasDatePicker: true
          hasTimePicker: root.hasTime
          dateTime: root.toDate ? root.toDate : new Date()

          onPrimaryButtonClicked: {
            root.toDate = dateTime
          }
          onClosed: toCalendarLoader.active = false
          Component.onCompleted: open()
        }
      }
    }
  }
}
