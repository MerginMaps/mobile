/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import mm 1.0 as MM

import "../components"
import "../form/components" as MMFormComponents

Column {
  id: root

  required property string layerId
  required property string layerName
  required property var filterController
  required property var vectorLayer

  spacing: __style.margin12

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

        // field label, not shown for text type since MMFilterTextInput has its own title
        MMText {
          width: parent.width
          visible: fieldDelegate.filterType !== "text"
          text: fieldDelegate.fieldDisplayName
          font: __style.p6
          color: __style.nightColor
        }

        // number range
        Row {
          width: parent.width
          spacing: __style.margin12
          visible: fieldDelegate.filterType === "number"

          property bool rangeInvalid: {
            let fromVal = parseFloat(fromNumberInput.text)
            let toVal = parseFloat(toNumberInput.text)
            return !isNaN(fromVal) && !isNaN(toVal) && fromVal > toVal
          }

          MMFilterTextInput {
            id: fromNumberInput
            width: (parent.width - __style.margin12) / 2
            type: "number"
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

          MMFilterTextInput {
            id: toNumberInput
            width: (parent.width - __style.margin12) / 2
            type: "number"
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

        // date range
        Row {
          id: dateRangeRow
          width: parent.width
          spacing: __style.margin12
          visible: fieldDelegate.filterType === "date"

          property bool rangeInvalid: {
            if (!fromDateInput.selectedDate || !toDateInput.selectedDate) return false
            return fromDateInput.selectedDate > toDateInput.selectedDate
          }

          Item {
            width: (parent.width - __style.margin12) / 2
            height: fromDateInput.height

            MMFilterTextInput {
              id: fromDateInput
              width: parent.width
              type: "date"

              property var selectedDate: null

              checked: selectedDate !== null

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
              errorMsg: dateRangeRow.rangeInvalid ? qsTr("\"From\" must be less than \"To\"") : ""

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

            MMFilterTextInput {
              id: toDateInput
              width: parent.width
              type: "date"

              property var selectedDate: null

              checked: selectedDate !== null

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

        // text filter
        MMFilterTextInput {
          width: parent.width
          visible: fieldDelegate.filterType === "text"
          type: "text"
          title: fieldDelegate.fieldDisplayName
          placeholderText: qsTr("Type to filter...")

          text: {
            let val = fieldDelegate.currentValue
            if (val !== null && val !== undefined && val !== "") return String(val)
            return ""
          }

          property bool initialized: false
          Component.onCompleted: initialized = true

          onTextChanged: {
            if (!initialized) return
            root.filterController.setTextFilter(root.layerId, fieldDelegate.fieldName, text)
          }
        }

        // boolean filter
        Item {
          id: boolItem

          width: parent.width
          height: __style.row50
          visible: fieldDelegate.filterType === "bool"

          // states: 0 = no filter, 1 = true, 2 = false
          property int boolState: {
            let val = fieldDelegate.currentValue
            if (val === null || val === undefined) return 0
            return (val === true || val === 1) ? 1 : 2
          }

          function cycleState() {
            boolState = (boolState + 1) % 3
            if (boolState === 0) {
              root.filterController.removeFieldFilter(root.layerId, fieldDelegate.fieldName)
            } else {
              root.filterController.setFieldFilter(root.layerId, fieldDelegate.fieldName, "bool", boolState === 1)
            }
          }

          Rectangle {
            anchors.fill: parent
            radius: __style.radius12
            color: boolItem.boolState !== 0 ? __style.filterGreenColor : __style.lightGreenColor
            border.color: boolItem.boolState !== 0 ? __style.grassColor : __style.polarColor
            border.width: boolItem.boolState !== 0 ? 1 * __dp : 0

            MMText {
              anchors {
                left: parent.left
                leftMargin: __style.margin20
                right: boolSwitch.left
                verticalCenter: parent.verticalCenter
              }
              text: {
                if (boolItem.boolState === 1) return qsTr("True")
                if (boolItem.boolState === 2) return qsTr("False")
                return ""
              }
              font: __style.p5
              color: __style.nightColor
            }

            MMSwitch {
              id: boolSwitch
              anchors {
                right: parent.right
                rightMargin: __style.margin20
                verticalCenter: parent.verticalCenter
              }
              checked: boolItem.boolState === 1
            }

            MouseArea {
              anchors.fill: parent
              z: 1
              onClicked: boolItem.cycleState()
            }
          }
        }

        // dropdown
        Item {
          width: parent.width
          height: dropdownInput.height
          visible: fieldDelegate.filterType === "dropdown"

          MMFilterTextInput {
            id: dropdownInput
            width: parent.width
            type: "dropdown"
            checked: text !== ""

            placeholderText: qsTr("Select...")
            text: {
              let texts = fieldDelegate.currentValueTexts
              if (!texts || texts.length === 0) return ""
              if (fieldDelegate.multiSelect && texts.length > 1) return qsTr("%1 selected").arg(texts.length)
              return texts.join(", ")
            }

            onTextClicked: dropdownDrawerLoader.active = true
            onRightContentClicked: {
              if (dropdownInput.text !== "") {
                root.filterController.setDropdownFilter(root.layerId, fieldDelegate.fieldName, [], fieldDelegate.multiSelect)
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
                onTriggered: populateOptions(internal.pendingSearchText)
              }

              function populateOptions(searchText) {
                let options = root.filterController.getDropdownOptions(root.vectorLayer, fieldDelegate.fieldName, searchText, 100)
                dropdownListModel.clear()
                for (let i = 0; i < options.length; i++) {
                  dropdownListModel.append(options[i])
                }
              }

              Component.onCompleted: {
                // QStringList from C++ does not support JS includes(), convert first
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
