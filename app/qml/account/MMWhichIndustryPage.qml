/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import mm 1.0 as MM

import "./components" as MMAccountComponents
import "../components"
import "../inputs"

MMPage {
  id: root

  property string selectedText: ""

  signal industrySelected( var selectedText )

  ListModel {
    id: sourceListModel

    Component.onCompleted: {
      var items = [
        { name: qsTr( "Agriculture" ), key: "agriculture", icon: __style.tractorIcon, colorx: __style.sunColor, color: "#F4CB46" },
        { name: qsTr( "Archaeology" ), key: "archaeology", icon: __style.archaeologyIcon, colorx: __style.sandColor, color: "#FFF4E2" },
        { name: qsTr( "Construction and engineering" ), key: "engineering", icon: __style.engineeringIcon, colorx: __style.roseColor, color: "#FFBABC" },
        { name: qsTr( "Electric utilities" ), key: "electricity", icon: __style.electricityIcon, colorx: __style.nightColor, color: "#12181F" },
        { name: qsTr( "Environmental protection" ), key: "environmental", icon: __style.environmentalIcon, colorx: __style.fieldColor, color: "#9BD1A9" },
        { name: qsTr( "Local governments" ), key: "gov", icon: __style.stateAndLocalIcon, colorx: __style.purpleColor, color: "#CCBDF5" },
        { name: qsTr( "Natural resources" ), key: "natural", icon: __style.naturalResourcesIcon, colorx: __style.earthColor, color: "#4D2A24" },
        { name: qsTr( "Telecom" ), key: "telecom", icon: __style.telecommunicationIcon, colorx: __style.deepOceanColor, color: "#1C324A" },
        { name: qsTr( "Transportation" ), key: "transportation", icon: __style.transportationIcon, colorx: __style.skyColor, color: "#A6CBF4" },
        { name: qsTr( "Water utilities" ), key: "water", icon: __style.waterResourcesIcon, colorx: __style.lightGreenColor, color: "#EFF5F3" }
      ];

      var otherItem = { name: qsTr( "Other" ), key: "other", icon: __style.otherIcon, colorx: __style.sunsetColor, color: "#FFB673" };

      shuffleAndAppend( sourceListModel, items );

      sourceListModel.append( otherItem );
    }
  }

  pageHeader {
    title: gridView.contentY > -10 * __dp ? qsTr( "Specify your industry" ) : ""
    backVisible: false

    rightItemContent: MMProgressBar {
      anchors.verticalCenter: parent.verticalCenter

      width: 60 * __dp
      height: 4 * __dp

      color: __style.grassColor
      progressColor: __style.forestColor

      position: 3/3
    }
  }

  pageBottomMarginPolicy: MMPage.BottomMarginPolicy.PaintBehindSystemBar

  pageContent: Item {
    width: parent.width
    height: parent.height

    GridView {
      id: gridView

      property real gridSpacing: __style.spacing12
      property real idealCellWidth: 150 * __dp

      width: parent.width
      height: parent.height

      clip: true

      cellHeight: 158 * __dp + gridSpacing
      cellWidth: idealCellWidth

      topMargin: __style.margin40

      // to reserve some space for the footer button
      bottomMargin: footerButton.height + __style.safeAreaBottom + __style.margin8 + __style.margin20

      onWidthChanged: {
        let colsCount = Math.floor( width / idealCellWidth )

        if ( colsCount < 1 ) {
          cellWidth = width
        }
        else {
          cellWidth = width / colsCount
        }
      }

      currentIndex: -1

      model: sourceListModel

      header: MMText {
        width: GridView.view.width

        text: internal.pageTitle

        font: __style.h3
        color: __style.forestColor

        wrapMode: Text.Wrap

        horizontalAlignment: Text.AlignHCenter

        bottomPadding: __style.margin40
      }

      delegate: Item {
        width: GridView.view.cellWidth - gridView.gridSpacing
        height: childrenRect.height

        MMAccountComponents.MMIconCheckBoxVertical {

          x: gridView.gridSpacing / 2

          width: parent.width

          sourceIcon: model.icon
          text: model.name
          bgColorIcon: model.color
          checked: gridView.currentIndex === index

          onClicked: {
            let optionUnchecked = gridView.currentIndex === index

            if ( optionUnchecked ) {
              gridView.currentIndex = -1
              root.selectedText = ""
            }
            else {
              gridView.currentIndex = index
              root.selectedText = model.key
            }

            if ( model.key === "other" && !optionUnchecked  ) {
                // add footer
                gridView.footer = specifyIndustryFooterComponent
                root.selectedText = ""
            }
            else {
              // remove footer
              gridView.footer = null
            }
          }
        }
      }
    }

    MMButton {
      id: footerButton

      width: parent.width

      anchors.bottom: parent.bottom
      anchors.bottomMargin: __style.safeAreaBottom + __style.margin8

      text: qsTr("Continue")

      enabled: {
        if ( gridView.currentIndex < 0 ) return false
        if ( ( gridView.model.get(gridView.currentIndex).key === "other" ) && root.selectedText === "" ) return false
        return true
      }

      onClicked: {
        if ( root.selectedText.length > 0 ) {
          root.industrySelected( root.selectedText )
        }
        else {
          __notificationModel.addError( internal.specifySourceText )
        }
      }
    }
  }

  QtObject {
    id: internal

    readonly property string pageTitle: qsTr( "Crafting your workspace: let us know your industry" )
    readonly property string specifyIndustryText : qsTr( "Please specify the industry" )
  }

  Component {
    id: specifyIndustryFooterComponent

    Column {

      width: GridView.view.width

      MMListSpacer { height: __style.margin20 }

      MMTextInput {
        width: parent.width

        title: qsTr( "Industry" )
        placeholderText: internal.specifyIndustryText

        onTextEdited: ( text ) => root.selectedText = text

        Component.onCompleted: textField.forceActiveFocus()
      }
    }
  }
}
