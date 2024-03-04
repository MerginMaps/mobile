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

import "./components"
import "../components"
import "../inputs"

Page {
  id: root

  width: parent.width

  property string selectedText: ""

  signal backClicked
  signal industrySelected(var selectedText)

  readonly property string headerTitle: qsTr("In which industry do you work?")
  readonly property real hPadding: width < __style.maxPageWidth
                                   ? 20 * __dp
                                   : (20 + (width - __style.maxPageWidth) / 2) * __dp

  readonly property string specifyIndustryText : qsTr("Please specify the industry")

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  MMPageHeader {
    id: header

    title: listView.contentY > -30 * __dp ? root.headerTitle : ""

    onBackClicked: root.backClicked()

    rightMarginShift: progressBar.width

    MMProgressBar {
      id: progressBar

      anchors.right: parent.right
      anchors.rightMargin: __style.pageMargins
      anchors.verticalCenter: parent.verticalCenter

      width: 60 * __dp
      height: 4 * __dp

      color: __style.grassColor
      progressColor: __style.forestColor
      position: 3/3
    }
  }

  Item {
    width: parent.width
    height: parent.height - (listView.model.count === listView.currentIndex + 1 ? header.height + 50 * __dp : 0)
    anchors.top: header.bottom
    anchors.topMargin: 20 * __dp

    GridView {
      id: listView

      property real spacing: 10 * __dp

      width: parent.width - 2 * root.hPadding
      anchors.horizontalCenter: parent.horizontalCenter
      height: parent.height - header.height
      clip: true
      cellHeight: 158 * __dp + listView.spacing

      onWidthChanged: {
        if(listView.width < 150 * __dp)
          listView.cellWidth = 150 * __dp + listView.spacing
        else if(listView.width < 250 * __dp)
          listView.cellWidth = listView.width + listView.spacing
        else
          listView.cellWidth = listView.width / 2
      }

      Component.onCompleted: currentIndex = -1

      model: ListModel {
        Component.onCompleted: {
          listView.model.append({name: qsTr("Agriculture"), key: "agriculture", icon: __style.tractorIcon, colorx: __style.sunColor, color: "#F4CB46"})
          listView.model.append({name: qsTr("Archaeology"), key: "archaeology", icon: __style.archaeologyIcon, colorx: __style.sandColor, color: "#FFF4E2"})
          listView.model.append({name: qsTr("Construction and engineering"), key: "engineering", icon: __style.engineeringIcon, colorx: __style.roseColor, color: "#FFBABC"})
          listView.model.append({name: qsTr("Electric utilities"), key: "electricity", icon: __style.electricityIcon, colorx: __style.nightColor, color: "#12181F"})
          listView.model.append({name: qsTr("Environmental protection"), key: "environmental", icon: __style.environmentalIcon, colorx: __style.fieldColor, color: "#9BD1A9"})
          listView.model.append({name: qsTr("Local governments"), key: "gov", icon: __style.stateAndLocalIcon, colorx: __style.purpleColor, color: "#CCBDF5"})
          listView.model.append({name: qsTr("Natural resources"), key: "natural", icon: __style.naturalResourcesIcon, colorx: __style.earthColor, color: "#4D2A24"})
          listView.model.append({name: qsTr("Telecom"), key: "telecom", icon: __style.telecommunicationIcon, colorx: __style.deepOceanColor, color: "#1C324A"})
          listView.model.append({name: qsTr("Transportation"), key: "transportation", icon: __style.transportationIcon, colorx: __style.skyColor, color: "#A6CBF4"})
          listView.model.append({name: qsTr("Water utilities"), key: "water", icon: __style.waterResourcesIcon, colorx: __style.lightGreenColor, color: "#EFF5F3"})
          listView.model.append({name: qsTr("Other"), key: "other", icon: __style.otherIcon, colorx: __style.sunsetColor, color: "#FFB673"})
        }
      }

      header: Text {
        id: listHeader

        width: root.width - 2 * root.hPadding
        padding: 20 * __dp
        text: root.headerTitle
        font: __style.h3
        color: __style.forestColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        lineHeight: 1.2
      }

      delegate: MMIconCheckBoxVertical {
        width: listView.cellWidth - listView.spacing
        sourceIcon: model.icon
        text: model.name
        bgColorIcon: model.color
        checked: listView.currentIndex === index

        onClicked: {
          listView.currentIndex = index
          if(listView.model.count === listView.currentIndex + 1) {
            listView.positionViewAtEnd()
            // for other you need to type string
            root.selectedText = ""
          } else {
            root.selectedText = model.key
          }
        }
      }

      footer: Column {
        width: root.width - 2 * root.hPadding
        topPadding: 20 * __dp
        visible: listView.model.count === listView.currentIndex + 1

        MMTextInput {
          title: qsTr("Source")
          placeholderText: root.specifyIndustryText
          onTextChanged: root.selectedText = text
          onVisibleChanged: if(visible) hasFocus = true
        }

        Item { width: 1; height: 60 * __dp }
      }
    }
  }

  MMButton {
    width: root.width - 2 * root.hPadding
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 20 * __dp
    text: qsTr("Continue")

    onClicked: {     
      if (root.selectedText.length > 0 ) {
        root.industrySelected(root.selectedText)
      } else {
        __notificationModel.addError( root.specifyIndustryText )
      }
    }
  }
}
