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

import "../components"
import "../inputs"

import notificationType 1.0

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

  MMHeader {
    id: header

    x: root.hPadding
    y: 20 * __dp
    width: root.width - 2 * root.hPadding
    headerTitle: listView.contentY > -30 * __dp ? root.headerTitle : ""
    step: 2

    onBackClicked: root.backClicked()
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
          listView.model.append({name: qsTr("Agriculture"), icon: __style.tractorIcon, colorx: __style.sunColor, color: "#F4CB46"})
          listView.model.append({name: qsTr("Archaeology"), icon: __style.archaeologyIcon, colorx: __style.sandColor, color: "#FFF4E2"})
          listView.model.append({name: qsTr("onstruction and engineering"), icon: __style.engineeringIcon, colorx: __style.roseColor, color: "#FFBABC"})
          listView.model.append({name: qsTr("Electric utilities"), icon: __style.electricityIcon, colorx: __style.nightColor, color: "#12181F"})
          listView.model.append({name: qsTr("Environmental protection"), icon: __style.environmentalIcon, colorx: __style.fieldColor, color: "#9BD1A9"})
          listView.model.append({name: qsTr("Local governments"), icon: __style.stateAndLocalIcon, colorx: __style.purpleColor, color: "#CCBDF5"})
          listView.model.append({name: qsTr("Natural resources"), icon: __style.naturalResourcesIcon, colorx: __style.earthColor, color: "#4D2A24"})
          listView.model.append({name: qsTr("Telecom"), icon: __style.telecommunicationIcon, colorx: __style.deepOceanColor, color: "#1C324A"})
          listView.model.append({name: qsTr("Transportation"), icon: __style.transportationIcon, colorx: __style.skyColor, color: "#A6CBF4"})
          listView.model.append({name: qsTr("Water utilities"), icon: __style.waterResourcesIcon, colorx: __style.lightGreenColor, color: "#EFF5F3"})
          listView.model.append({name: qsTr("Other"), icon: __style.otherIcon, colorx: __style.sunsetColor, color: "#FFB673"})
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
            root.selectedText = model.name
          }
        }
      }

      footer: Column {
        width: root.width - 2 * root.hPadding
        topPadding: 20 * __dp
        visible: listView.model.count === listView.currentIndex + 1

        MMInputEditor {
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
        __notificationModel.add(
          root.specifyIndustryText,
          3,
          NotificationType.Error,
          NotificationType.None
        )
      }
    }
  }
}
