/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import "."  // import InputStyle singleton
import "./components"

Rectangle {
  id: root
  visible: true

  signal backClicked
  signal subscribeClicked

  //! If true and component is visible, busy indicator suppose to be on. Currently used only while fetching a recommendedPlan
  property bool isBusy: __purchasing.individualPlan.id === "" || __purchasing.professionalPlan.id === ""

  onVisibleChanged: {
    subscribeBusyIndicator.running = root.visible && root.isBusy
  }

  Connections {
    target: __purchasing
    onIndividualPlanChanged: {
      if (!root.isBusy && root.visible) {
        subscribeBusyIndicator.running = false
      }
    }
    onProfessionalPlanChanged: {
      if (!root.isBusy && root.visible) {
        subscribeBusyIndicator.running = false
      }
    }
  }

  BusyIndicator {
    id: subscribeBusyIndicator
    width: root.width/8
    height: width
    running: false
    visible: running
    anchors.centerIn: root
    z: root.z + 1
  }

  // header
  PanelHeader {
    id: header
    height: InputStyle.rowHeightHeader
    width: parent.width
    color: InputStyle.clrPanelMain
    rowHeight: InputStyle.rowHeightHeader
    titleText: qsTr("Subscribe")

    onBack: backClicked()
    withBackButton: true
  }

  Image {
    id: merginLogo
    anchors.top: header.bottom
    antialiasing: true
    source: InputStyle.merginColorIcon
    height: InputStyle.rowHeightHeader
    width: parent.width
    sourceSize.width: width
    sourceSize.height: height
    fillMode: Image.PreserveAspectFit
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
  }

  Column {
    anchors.top: merginLogo.bottom
    width: parent.width
    anchors.horizontalCenter: parent.horizontalCenter
    TabBar {
        id: bar
        width: root.width - 2 * InputStyle.rowHeightHeader
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 5

        TabButton {
            id: individualTabButton

            contentItem: Text {
              color: bar.currentIndex === 0 ? "white" : InputStyle.highlightColor
              text: qsTr("Individual")
              horizontalAlignment: Text.AlignHCenter
              verticalAlignment: Text.AlignVCenter
              font.pixelSize: InputStyle.fontPixelSizeNormal
            }

            background: Rectangle {
              color: bar.currentIndex === 0 ? InputStyle.highlightColor: "white"
              border.color: InputStyle.highlightColor
              border.width: 1
            }
        }
        TabButton {
            id: professionalTabButton

            contentItem: Text {
              color: bar.currentIndex === 1 ? "white" : InputStyle.highlightColor
              text: qsTr("Professional")
              horizontalAlignment: Text.AlignHCenter
              verticalAlignment: Text.AlignVCenter
              font.pixelSize: InputStyle.fontPixelSizeNormal
            }

            background: Rectangle {
              color: bar.currentIndex === 1 ? InputStyle.highlightColor: "white"
              border.color: InputStyle.highlightColor
              border.width: 1
            }
        }
    }

    SwipeView {
        id: view

        currentIndex: bar.currentIndex
        anchors.top: bar.bottom
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter

        SubscribePlanItem {
            id: individualTab
            plan: __purchasing.individualPlan
            name: qsTr("Individual")
            onSubscribeClicked: {
               __purchasing.purchase( __purchasing.individualPlan.id )
              root.subscribeClicked()
            }
        }
        SubscribePlanItem {
            id: professionalTab
            plan: __purchasing.professionalPlan
            name: qsTr("Professional")
            onSubscribeClicked: {
               __purchasing.purchase( __purchasing.professionalPlan.id )
              root.subscribeClicked()
            }
        }

    }
  }

  TextHyperlink {
    id: textNotice
    width: parent.width
    height: InputStyle.rowHeightHeader
    anchors.bottom: parent.bottom
    anchors.bottomMargin: InputStyle.rowHeightHeader/2
    text: qsTr("Your Mergin subscription plan will renew automatically. You can cancel or change it at any time. %1Learn More%2")
              .arg("<a href='" + __inputHelp.merginSubscriptionDetailsLink + "'>")
              .arg("</a>")
  }
}
