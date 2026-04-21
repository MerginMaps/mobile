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

Column {
  id: root

  width: parent.width
  spacing: __style.margin8

  required property string filterName
  required property var currentValue
  required property bool hasTime

  property bool rangeInvalid: {
    if ( !currentValue || !currentValue[0] || !currentValue[1] ){
      return false
    }
    return currentValue[0] > currentValue[1]
  }

  MMText {
    width: parent.width
    text: root.filterName
    font: __style.p6
    color: __style.nightColor
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
        placeholderText: qsTr( "From" )
        errorMsg: root.rangeInvalid ? qsTr( "\"From\" must be sooner than \"To\"" ) : ""
        text: {
          if ( !root.currentValue || !root.currentValue[0] ) return ""
          if ( root.hasTime ) return Qt.formatDateTime( root.currentValue[0] )
          return Qt.formatDate( root.currentValue[0] )
        }

        onTextClicked: fromCalendarLoader.active = true
        onRightContentClicked: {
          if (checked) {
            textField.clear()
            checked = false
            if ( root.currentValue[1] ){
              root.currentValue = [undefined, root.currentValue[1]]
            } else {
              root.currentValue = undefined
            }
            root.currentValueChanged()
          } else {
            let currentTimestamp = new Date()

            if (root.hasTime) {
              text = Qt.formatDateTime(currentTimestamp)
            } else {
              text = Qt.formatDate(currentTimestamp)
            }

            if (!root.hasTime) {
              currentTimestamp.setHours(0, 0, 0, 0)
            } else {
              currentTimestamp.setSeconds(0, 0)
            }

            if (!root.currentValue) {
              root.currentValue = [currentTimestamp, undefined]
            } else {
              root.currentValue[0] = currentTimestamp
            }
            root.currentValueChanged()
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
          dateTime: root.currentValue && root.currentValue[0] ? root.currentValue[0] : new Date()

          onPrimaryButtonClicked: {
            let currentTimestamp = dateTime

            if (!root.hasTime) {
              currentTimestamp.setHours(0, 0, 0, 0)
            } else {
              currentTimestamp.setSeconds(0, 0)
            }

            if (!root.currentValue){
              root.currentValue = [currentTimestamp, undefined]
            } else {
              root.currentValue[0] = currentTimestamp
              root.currentValueChanged()
            }

            fromDateInput.text = root.hasTime ? Qt.formatDateTime(dateTime) : Qt.formatDate(dateTime)
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
        placeholderText: qsTr( "To" )
        errorMsg: root.rangeInvalid ? qsTr( "\"From\" must be sooner than \"To\"" ) : ""
        text: {
          if ( !root.currentValue || !root.currentValue[1] ) return ""
          if ( root.hasTime ) return Qt.formatDateTime( root.currentValue[1] )
          return Qt.formatDate( root.currentValue[1] )
        }

        onTextClicked: toCalendarLoader.active = true
        onRightContentClicked: {
          if (checked) {
            textField.clear()
            checked = false
            if ( root.currentValue[0] ){
              root.currentValue = [root.currentValue[0], undefined]
            } else {
              root.currentValue = undefined
            }
            root.currentValueChanged()

          } else {
            let currentTimestamp = new Date()

            if (root.hasTime) {
              text = Qt.formatDateTime(currentTimestamp)
            } else {
              text = Qt.formatDate(currentTimestamp)
            }

            if (!root.hasTime) {
              currentTimestamp.setHours(0, 0, 0, 0)
            } else {
              currentTimestamp.setSeconds(0, 0)
            }

            if (!root.currentValue) {
              root.currentValue = [undefined, currentTimestamp]
            } else {
              root.currentValue[1] = currentTimestamp
              root.currentValueChanged()
            }
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
          dateTime: root.currentValue && root.currentValue[1] ? root.currentValue[1] : new Date()

          onPrimaryButtonClicked: {
            let currentTimestamp = dateTime

            if (!root.hasTime) {
              currentTimestamp.setHours(0, 0, 0, 0)
            } else {
              currentTimestamp.setSeconds(0, 0)
            }

            if (!root.currentValue){
              root.currentValue = [undefined, currentTimestamp]
            } else {
              root.currentValue[1] = currentTimestamp
            }
            root.currentValueChanged()

            toDateInput.text = root.hasTime ? Qt.formatDateTime(dateTime) : Qt.formatDate(dateTime)
          }
          onClosed: toCalendarLoader.active = false
          Component.onCompleted: open()
        }
      }
    }
  }
}
