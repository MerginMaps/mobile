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
import QtQuick.Controls

import "../components" as MMComponents

//
// TODO: rename this file to "MMFiltersDrawer"!
//

MMComponents.MMDrawer {
  id: root

  modal: false
  interactive: false
  closePolicy: Popup.CloseOnEscape

  drawerBottomMargin: 0

  dropShadow: true

  background: Rectangle {
    color: __style.polarColor
    radius: __style.radius20

    layer.enabled: root.dropShadow
    layer.effect: MMComponents.MMShadow {}

    Rectangle {
      color: __style.polarColor
      width: parent.width
      height: parent.height / 2
      y: parent.height / 2
    }
  }

  Component.onCompleted: {
    root.open()
  }

  QtObject {
    id: internal
    
    property var filterValues: ({})
  }

  drawerHeader.title: qsTr( "Filters" )
  drawerHeader.titleFont: __style.t2

  drawerHeader.topLeftItemContent: MMComponents.MMButton {
    type: MMButton.Types.Primary
    size: MMButton.Sizes.Small
    text: qsTr( "Reset" )
    fontColor: __style.grapeColor
    bgndColor: __style.negativeLightColor
    bgndColorHover: __style.grapeColor
    fontColorHover: __style.negativeLightColor

    anchors {
      left: parent.left
      leftMargin: __style.pageMargins + __style.safeAreaLeft
      verticalCenter: parent.verticalCenter
    }

    onClicked: {
      internal.filterValues = {}

      __activeProject.filterController.clearAllFilters()

      inputRepeater.model = null
      inputRepeater.model = __activeProject.filterController.getFilters()
    }
  }

  drawerContent: Item {
    width: parent.width
    height: root.drawerContentAvailableHeight

    MMComponents.MMScrollView {
      id: scrollView

      width: parent.width
      height: parent.height

      Column {
        id: contentColumn

        width: scrollView.availableWidth
        spacing: __style.margin20

        MMComponents.MMText {
          width: parent.width
          visible: inputRepeater.count === 0
          text: qsTr( "No filtering" )
          font: __style.p4
          color: __style.mediumGreyColor
          horizontalAlignment: Text.AlignHCenter
        }

        Repeater {
          id: inputRepeater

          model: __activeProject.filterController.getFilters()

          delegate: Loader {
            id: fieldLoader

            required property var modelData

            width: contentColumn.width

            Component.onCompleted: {
              let props = { filterName: modelData.filterName }

              const currentValue = modelData.value ? modelData.value : internal.filterValues[modelData.filterId]
              props['currentValue'] = currentValue

              const filterType = modelData.filterType

              if ( filterType === FieldFilter.TextFilter )
              {
                setSource( "components/MMFilterTextEditor.qml", props )
              }
              else if ( filterType === FieldFilter.NumberFilter )
              {
                setSource( "components/MMFilterRangeInput.qml", props )
              }
              else if ( filterType === FieldFilter.DateFilter )
              {
                props['hasTime'] = __activeProject.filterController.isDateFilterDateTime(modelData.filterId)
                setSource( "components/MMFilterDateRange.qml", props )
              }
              else if ( filterType === FieldFilter.CheckboxFilter )
              {
                const checkboxConfig = __activeProject.filterController.getCheckboxConfiguration( modelData.filterId )
                props["customLabelForTrue"] = checkboxConfig["customLabelForTrue"] ?? ""
                props["customLabelForFalse"] = checkboxConfig["customLabelForFalse"] ?? ""
                props["customValueForTrue"] = checkboxConfig["customValueForTrue"]
                props["customValueForFalse"] = checkboxConfig["customValueForFalse"]
                setSource( "components/MMFilterBoolInput.qml", props )
              }
              else if ( filterType === FieldFilter.SingleSelectFilter || filterType === FieldFilter.MultiSelectFilter )
              {
                // TODO: might be worth moving this logic to C++

                props['isMultiSelect'] = filterType === FieldFilter.MultiSelectFilter

                const dropdownConfig = __activeProject.filterController.getDropdownConfiguration( modelData.filterId )

                if ( !Object.keys( dropdownConfig ).length )
                {
                  __inputUtils.log( "Filters", "Received invalid config for dropdown filter " + modelData.filterName )
                  return;
                }

                if ( dropdownConfig["type"] === "unique_values" )
                {
                  props["vectorLayerId"] = dropdownConfig["layer_id"]
                  props["fieldName"] = dropdownConfig["field_name"]
                  setSource( "components/MMFilterDropdownUniqueValuesInput.qml", props )
                }
                else if ( dropdownConfig["type"] === "value_relation" )
                {
                  props["widgetConfig"] = dropdownConfig["config"]
                  setSource( "components/MMFilterDropdownValueRelationInput.qml", props )
                }
                else if ( dropdownConfig["type"] === "value_map" )
                {
                  props["widgetConfig"] = dropdownConfig["config"]
                  setSource( "components/MMFilterDropdownValueMapInput.qml", props )
                }
              }
            }

            Connections {
              target: fieldLoader.item
              ignoreUnknownSignals: true

              function onCurrentValueChanged() {
                internal.filterValues[fieldLoader.modelData.filterId] = fieldLoader.item.currentValue
              }
            }
          }
        }

        Item {
          width: parent.width
          height: showResultsButton.height + __style.margin12 + __style.safeAreaBottom
        }
      }
    }

    MMComponents.MMButton {
      id: showResultsButton

      z: 1
      width: parent.width

      anchors {
        bottom: parent.bottom
        bottomMargin: __style.margin8 + __style.safeAreaBottom
        left: parent.left
        right: parent.right
      }

      text: qsTr( "Apply filters" )

      onClicked: {
        __activeProject.filterController.processFilters(internal.filterValues)
        root.close()
      }
    }
  }
}
