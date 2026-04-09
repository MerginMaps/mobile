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

  property bool filtersApplied: false

  Component.onCompleted: {
    internal.refreshLayers()
    root.open()
  }

  onClosed: {
    if ( !filtersApplied ) {
      __activeProject.filterController.discardPendingChanges()
    }
    filtersApplied = false
  }

  QtObject {
    id: internal
    property var vectorLayers: []
    property var allFields: []

    function refreshLayers() {
      vectorLayers = __activeProject.filterController.getVectorLayers()
      let fields = []
      for ( let i = 0; i < vectorLayers.length; i++ ) {
        let lyr = vectorLayers[i]
        let lyrFields = __activeProject.filterController.getFilterableFields( lyr.layer )
        for ( let j = 0; j < lyrFields.length; j++ ) {
          let f = lyrFields[j]
          fields.push({
            layerId: lyr.layerId,
            vectorLayer: lyr.layer,
            name: f.name,
            displayName: f.displayName || f.name,
            filterType: f.filterType || "text",
            currentValue: f.currentValue !== undefined ? f.currentValue : null,
            currentValueTo: f.currentValueTo !== undefined ? f.currentValueTo : null,
            hasTime: !!f.hasTime,
            multiSelect: !!f.multiSelect,
            currentValueTexts: f.currentValueTexts || [],
            boolTrueLabel: f.boolTrueLabel || "",
            boolFalseLabel: f.boolFalseLabel || "",
            boolCheckedValue: f.boolCheckedValue !== undefined ? f.boolCheckedValue : null,
            boolUncheckedValue: f.boolUncheckedValue !== undefined ? f.boolUncheckedValue : null
          })
        }
      }
      // Setting to [] first forces the Repeater to destroy all existing delegates.
      // Qt.callLater ensures the two assignments are processed in separate event
      // loop iterations, so the recreation picks up the fresh field values.
      allFields = []
      Qt.callLater( function() { allFields = fields } )
    }
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
      __activeProject.filterController.applyFiltersToAllLayers()
      root.filtersApplied = true
      internal.refreshLayers()
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
          visible: internal.allFields.length === 0
          text: qsTr( "No filtering" )
          font: __style.p4
          color: __style.mediumGreyColor
          horizontalAlignment: Text.AlignHCenter
        }

        Repeater {
          model: internal.allFields

          delegate: Loader {
            id: fieldLoader

            required property var modelData

            width: contentColumn.width

            Component.onCompleted: {
              let base = {
                fieldDisplayName: modelData.displayName,
                fieldLayerId:     modelData.layerId,
                fieldName:        modelData.name,
                currentValue:     modelData.currentValue
              }
              switch ( modelData.filterType ) {
                case "text":
                  setSource( "components/MMFilterTextEditor.qml", base )
                  break
                case "number":
                  setSource( "components/MMFilterRangeInput.qml", Object.assign( {}, base, {
                    currentValueTo: modelData.currentValueTo
                  }))
                  break
                case "date":
                  setSource( "components/MMFilterDateRange.qml", Object.assign( {}, base, {
                    currentValueTo: modelData.currentValueTo,
                    hasTime:        modelData.hasTime
                  }))
                  break
                case "bool":
                  setSource( "components/MMFilterBoolInput.qml", Object.assign( {}, base, {
                    boolTrueLabel:    modelData.boolTrueLabel,
                    boolFalseLabel:   modelData.boolFalseLabel,
                    boolCheckedValue: modelData.boolCheckedValue,
                    boolUncheckedValue: modelData.boolUncheckedValue
                  }))
                  break
                case "dropdown":
                  setSource( "components/MMFilterDropdownEditor.qml", Object.assign( {}, base, {
                    currentValueTexts: modelData.currentValueTexts,
                    multiSelect:       modelData.multiSelect,
                    vectorLayer:       modelData.vectorLayer
                  }))
                  break
              }
            }

            Connections {
              target: fieldLoader.item
              ignoreUnknownSignals: true
              function onRefreshRequested() { internal.refreshLayers() }
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
        __activeProject.filterController.applyFiltersToAllLayers()
        root.filtersApplied = true
        root.close()
      }
    }
  }

  Connections {
    target: __activeProject

    function onProjectReloaded( qgsProject ) {
      internal.refreshLayers()
    }
  }
}
