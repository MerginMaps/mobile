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
    bgndColorHover: __style.negativeLightColor
    fontColorHover: __style.grapeColor
    iconColorHover: __style.grapeColor

    anchors {
      left: parent.left
      leftMargin: __style.pageMargins + __style.safeAreaLeft
      verticalCenter: parent.verticalCenter
    }

    onClicked: {
      __activeProject.filterController.clearAllFilters()
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
              let base = {
                filterName:         modelData.filterName,
                filterId:           modelData.filterId,
                currentValue:       modelData.value
              }

              const filterType = modelData.filterType

              if ( filterType === FieldFilter.TextFilter )
              {
                setSource( "components/MMFilterTextEditor.qml", base )
              }
              else if ( filterType === FieldFilter.NumberFilter )
              {
                setSource( "components/MMFilterRangeInput.qml", base )
              }
              else if ( filterType === FieldFilter.DateFilter )
              {
                setSource( "components/MMFilterDateRange.qml", base )
              }
              else if ( filterType === FieldFilter.CheckboxFilter )
              {
                setSource( "components/MMFilterBoolInput.qml", base )
              }
              else if ( filterType === FieldFilter.SingleSelectFilter || filterType === FieldFilter.MultiSelectFilter )
              {
                // TODO: might be worth moving this logic to C++

                const isMulti = filterType === FieldFilter.MultiSelectFilter
                base['isMultiSelect'] = isMulti

                const dropdownConfig = __activeProject.filterController.getDropdownConfiguration( modelData.filterId )
                console.log( "--> dropdown config:", JSON.stringify( dropdownConfig ) )

                if ( !Object.keys( dropdownConfig ).length )
                {
                  __inputUtils.log( "Filters", "Received invalid config for dropdown filter " + modelData.filterName )
                  return;
                }

                if ( dropdownConfig["type"] === "unique_values" )
                {
                  base["vectorLayerId"] = dropdownConfig["layer_id"]
                  base["fieldName"] = dropdownConfig["field_name"]
                  setSource( "components/MMFilterDropdownUniqueValuesInput.qml", base )
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
