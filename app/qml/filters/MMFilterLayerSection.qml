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

import mm 1.0 as MM

import "../components"
import "../components/private" as MMPrivateComponents
import "../inputs"
import "../form/components" as MMFormComponents

Column {
  id: root

  required property string layerId
  required property string layerName
  required property var filterController
  required property var vectorLayer

  spacing: __style.margin12

  // Layer header
  MMText {
    width: root.width
    text: root.layerName
    font: __style.t2
    color: __style.forestColor
  }

  Rectangle {
    width: root.width
    height: 1
    color: __style.greyColor
  }

  // Filter fields
  Column {
    width: root.width
    spacing: __style.margin16

    Repeater {
      id: fieldsRepeater

      model: root.vectorLayer ? root.filterController.getFilterableFields(root.vectorLayer) : []

      delegate: Column {
        id: fieldDelegate

        property var fieldInfo: modelData
        property string fieldName: fieldInfo ? fieldInfo.name : ""
        property string fieldDisplayName: fieldInfo ? (fieldInfo.displayName || fieldInfo.name) : ""
        property string filterType: fieldInfo ? fieldInfo.filterType : "text"
        property var currentValue: fieldInfo ? fieldInfo.currentValue : null
        property var currentValueTo: fieldInfo ? fieldInfo.currentValueTo : null
        property bool hasTime: fieldInfo ? !!fieldInfo.hasTime : false
        property bool multiSelect: fieldInfo ? !!fieldInfo.multiSelect : false
        property var currentValueTexts: fieldInfo ? fieldInfo.currentValueTexts : []

        width: root.width
        spacing: __style.margin8

        // Number filter (range) - shown for number type
        Column {
          width: parent.width
          spacing: __style.margin8
          visible: fieldDelegate.filterType === "number"

          MMText {
            text: fieldDelegate.fieldDisplayName
            font: __style.p6
            color: __style.nightColor
          }

          Row {
            width: parent.width
            spacing: __style.margin12

            property bool rangeInvalid: {
              let fromVal = parseFloat(fromNumberInput.text)
              let toVal = parseFloat(toNumberInput.text)
              return !isNaN(fromVal) && !isNaN(toVal) && fromVal > toVal
            }

            MMTextInput {
              id: fromNumberInput
              width: (parent.width - __style.margin12) / 2
              placeholderText: qsTr("From")
              text: fieldDelegate.currentValue !== null && fieldDelegate.currentValue !== undefined ? String(fieldDelegate.currentValue) : ""
              errorMsg: parent.rangeInvalid ? qsTr("\"From\" must be less than \"To\"") : ""

              property bool initialized: false
              Component.onCompleted: initialized = true

              onTextChanged: {
                if (!initialized || !toNumberInput.initialized) return
                root.filterController.setNumberFilter(root.layerId, fieldDelegate.fieldName, text, toNumberInput.text)
              }
            }

            MMTextInput {
              id: toNumberInput
              width: (parent.width - __style.margin12) / 2
              placeholderText: qsTr("To")
              text: fieldDelegate.currentValueTo !== null && fieldDelegate.currentValueTo !== undefined ? String(fieldDelegate.currentValueTo) : ""

              property bool initialized: false
              Component.onCompleted: initialized = true

              onTextChanged: {
                if (!initialized || !fromNumberInput.initialized) return
                root.filterController.setNumberFilter(root.layerId, fieldDelegate.fieldName, fromNumberInput.text, text)
              }
            }
          }
        }

        // Date filter (range) - shown for date type
        Column {
          width: parent.width
          spacing: __style.margin8
          visible: fieldDelegate.filterType === "date"

          MMText {
            text: fieldDelegate.fieldDisplayName
            font: __style.p6
            color: __style.nightColor
          }

          Row {
            id: dateRangeRow
            width: parent.width
            spacing: __style.margin12

            property bool rangeInvalid: {
              if (!fromDateInput.selectedDate || !toDateInput.selectedDate) return false
              return fromDateInput.selectedDate > toDateInput.selectedDate
            }

            Item {
              width: (parent.width - __style.margin12) / 2
              height: fromDateInput.height

              MMPrivateComponents.MMBaseSingleLineInput {
                id: fromDateInput
                width: parent.width

                property var selectedDate: null

                Component.onCompleted: {
                  let val = fieldDelegate.currentValue
                  if (val !== null && val !== undefined) {
                    let d = new Date(val)
                    if (!isNaN(d.getTime())) selectedDate = d
                  }
                }

                placeholderText: qsTr("From")
                text: {
                  if (!selectedDate) return ""
                  if (fieldDelegate.hasTime) return Qt.formatDateTime(selectedDate, Qt.DefaultLocaleShortDate)
                  return Qt.formatDate(selectedDate, Qt.DefaultLocaleShortDate)
                }
                textField.readOnly: true
                errorMsg: dateRangeRow.rangeInvalid ? qsTr("\"From\" must be less than \"To\"") : ""

                rightContent: MMIcon {
                  size: __style.icon24
                  source: fromDateInput.selectedDate ? __style.closeIcon : __style.calendarIcon
                  color: fromDateInput.iconColor
                }

                onTextClicked: fromCalendarLoader.active = true
                onRightContentClicked: {
                  if (fromDateInput.selectedDate) {
                    fromDateInput.selectedDate = null
                    let toDate = toDateInput.selectedDate ? toDateInput.selectedDate : null
                    root.filterController.setDateFilter(root.layerId, fieldDelegate.fieldName, null, toDate, fieldDelegate.hasTime)
                  }
                  else {
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
                  hasTimePicker: fieldDelegate.hasTime
                  dateTime: fromDateInput.selectedDate ? fromDateInput.selectedDate : new Date()

                  onPrimaryButtonClicked: {
                    fromDateInput.selectedDate = dateTime
                    let toDate = toDateInput.selectedDate ? toDateInput.selectedDate : null
                    root.filterController.setDateFilter(root.layerId, fieldDelegate.fieldName, dateTime, toDate, fieldDelegate.hasTime)
                  }

                  onClosed: fromCalendarLoader.active = false

                  Component.onCompleted: open()
                }
              }
            }

            Item {
              width: (parent.width - __style.margin12) / 2
              height: toDateInput.height

              MMPrivateComponents.MMBaseSingleLineInput {
                id: toDateInput
                width: parent.width

                property var selectedDate: null

                Component.onCompleted: {
                  let val = fieldDelegate.currentValueTo
                  if (val !== null && val !== undefined) {
                    let d = new Date(val)
                    if (!isNaN(d.getTime())) selectedDate = d
                  }
                }

                placeholderText: qsTr("To")
                text: {
                  if (!selectedDate) return ""
                  if (fieldDelegate.hasTime) return Qt.formatDateTime(selectedDate, Qt.DefaultLocaleShortDate)
                  return Qt.formatDate(selectedDate, Qt.DefaultLocaleShortDate)
                }
                textField.readOnly: true

                rightContent: MMIcon {
                  size: __style.icon24
                  source: toDateInput.selectedDate ? __style.closeIcon : __style.calendarIcon
                  color: toDateInput.iconColor
                }

                onTextClicked: toCalendarLoader.active = true
                onRightContentClicked: {
                  if (toDateInput.selectedDate) {
                    toDateInput.selectedDate = null
                    let fromDate = fromDateInput.selectedDate ? fromDateInput.selectedDate : null
                    root.filterController.setDateFilter(root.layerId, fieldDelegate.fieldName, fromDate, null, fieldDelegate.hasTime)
                  }
                  else {
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
                  hasTimePicker: fieldDelegate.hasTime
                  dateTime: toDateInput.selectedDate ? toDateInput.selectedDate : new Date()

                  onPrimaryButtonClicked: {
                    toDateInput.selectedDate = dateTime
                    let fromDate = fromDateInput.selectedDate ? fromDateInput.selectedDate : null
                    root.filterController.setDateFilter(root.layerId, fieldDelegate.fieldName, fromDate, dateTime, fieldDelegate.hasTime)
                  }

                  onClosed: toCalendarLoader.active = false

                  Component.onCompleted: open()
                }
              }
            }
          }
        }

        // Text filter - shown for text type
        MMTextInput {
          id: textFilterInput
          width: parent.width
          visible: fieldDelegate.filterType === "text"
          title: fieldDelegate.fieldDisplayName
          placeholderText: qsTr("Type to filter...")

          // Explicitly handle undefined/null values
          text: {
            let val = fieldDelegate.currentValue
            if (val !== null && val !== undefined && val !== "") {
              return String(val)
            }
            return ""
          }

          property bool initialized: false

          Component.onCompleted: initialized = true

          onTextChanged: {
            if (!initialized) return
            // Pass raw text to C++ - validation happens there
            root.filterController.setTextFilter(root.layerId, fieldDelegate.fieldName, text)
          }
        }

        // Dropdown filter - shown for ValueMap/ValueRelation fields
        Column {
          width: parent.width
          spacing: __style.margin8
          visible: fieldDelegate.filterType === "dropdown"

          MMText {
            text: fieldDelegate.fieldDisplayName
            font: __style.p6
            color: __style.nightColor
          }

          Item {
            width: parent.width
            height: dropdownInput.height

            MMPrivateComponents.MMBaseSingleLineInput {
              id: dropdownInput
              width: parent.width
              textField.readOnly: true

              placeholderText: qsTr("Select...")
              text: {
                let texts = fieldDelegate.currentValueTexts
                if (!texts || texts.length === 0) return ""
                if (fieldDelegate.multiSelect && texts.length > 1) {
                  return qsTr("%1 selected").arg(texts.length)
                }
                return texts.join(", ")
              }

              rightContent: MMIcon {
                size: __style.icon24
                source: dropdownInput.text !== "" ? __style.closeIcon : __style.arrowDownIcon
                color: dropdownInput.iconColor
              }

              onTextClicked: dropdownDrawerLoader.active = true
              onRightContentClicked: {
                if (dropdownInput.text !== "") {
                  root.filterController.setDropdownFilter(root.layerId, fieldDelegate.fieldName, [], fieldDelegate.multiSelect)
                  // Refresh the fields model to clear currentValueTexts
                  fieldsRepeater.model = root.vectorLayer ? root.filterController.getFilterableFields(root.vectorLayer) : []
                }
                else {
                  dropdownDrawerLoader.active = true
                }
              }
            }

            Loader {
              id: dropdownDrawerLoader
              active: false
              sourceComponent: dropdownDrawerComponent
            }

            Component {
              id: dropdownDrawerComponent

              MMListMultiselectDrawer {
                id: dropdownDrawer

                drawerHeader.title: fieldDelegate.fieldDisplayName
                multiSelect: fieldDelegate.multiSelect
                withSearch: true
                showFullScreen: fieldDelegate.multiSelect

                list.model: ListModel { id: dropdownListModel }

                onSearchTextChanged: function(searchText) {
                  internal.pendingSearchText = searchText
                  searchDebounceTimer.restart()
                }

                onSelectionFinished: function(selectedItems) {
                  root.filterController.setDropdownFilter(root.layerId, fieldDelegate.fieldName, selectedItems, fieldDelegate.multiSelect)
                  // Refresh the fields model to update currentValueTexts
                  fieldsRepeater.model = root.vectorLayer ? root.filterController.getFilterableFields(root.vectorLayer) : []
                  close()
                }

                onClosed: dropdownDrawerLoader.active = false

                QtObject {
                  id: internal
                  property string pendingSearchText: ""
                }

                Timer {
                  id: searchDebounceTimer
                  interval: 300
                  repeat: false
                  onTriggered: {
                    populateOptions(internal.pendingSearchText)
                  }
                }

                function populateOptions(searchText) {
                  let options = root.filterController.getDropdownOptions(root.vectorLayer, fieldDelegate.fieldName, searchText, 100)
                  dropdownListModel.clear()
                  for (let i = 0; i < options.length; i++) {
                    dropdownListModel.append(options[i])
                  }
                }

                Component.onCompleted: {
                  // Set selected imperatively — QStringList from C++ needs
                  // conversion to a plain JS array for includes() to work
                  let val = fieldDelegate.currentValue
                  if (val && val.length > 0) {
                    let arr = []
                    for (let i = 0; i < val.length; i++) {
                      arr.push(String(val[i]))
                    }
                    selected = arr
                  }
                  populateOptions("")
                  open()
                }
              }
            }
          }
        }
      }
    }
  }

  // Spacer at bottom
  Item {
    width: root.width
    height: __style.margin20
  }
}
