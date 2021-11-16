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
import "components"

Rectangle {
  id: root

  //! If true and component is visible, busy indicator suppose to be on. Currently used only while fetching a recommendedPlan
  property bool isBusy: __purchasing.individualPlan.id === "" || __purchasing.professionalPlan.id === ""

  visible: true

  signal backClicked
  signal subscribeClicked

  onVisibleChanged: {
    subscribeBusyIndicator.running = root.visible && root.isBusy;
  }

  Connections {
    target: __purchasing

    onIndividualPlanChanged: {
      if (!root.isBusy && root.visible) {
        subscribeBusyIndicator.running = false;
      }
    }
    onProfessionalPlanChanged: {
      if (!root.isBusy && root.visible) {
        subscribeBusyIndicator.running = false;
      }
    }
  }
  BusyIndicator {
    id: subscribeBusyIndicator
    anchors.centerIn: root
    height: width
    running: false
    visible: running
    width: root.width / 8
    z: root.z + 1
  }

  // header
  PanelHeader {
    id: header
    color: InputStyle.clrPanelMain
    height: InputStyle.rowHeightHeader
    rowHeight: InputStyle.rowHeightHeader
    titleText: qsTr("Subscribe")
    width: parent.width
    withBackButton: true

    onBack: backClicked()
  }
  Image {
    id: merginLogo
    anchors.top: header.bottom
    antialiasing: true
    fillMode: Image.PreserveAspectFit
    height: InputStyle.rowHeightHeader
    horizontalAlignment: Text.AlignHCenter
    source: InputStyle.merginColorIcon
    sourceSize.height: height
    sourceSize.width: width
    verticalAlignment: Text.AlignVCenter
    width: parent.width
  }
  Column {
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: merginLogo.bottom
    width: parent.width

    TabBar {
      id: bar
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: 5
      width: root.width - 2 * InputStyle.rowHeightHeader

      TabButton {
        id: individualTabButton
        background: Rectangle {
          border.color: InputStyle.highlightColor
          border.width: 1
          color: bar.currentIndex === 0 ? InputStyle.highlightColor : "white"
        }
        contentItem: Text {
          color: bar.currentIndex === 0 ? "white" : InputStyle.highlightColor
          font.pixelSize: InputStyle.fontPixelSizeNormal
          horizontalAlignment: Text.AlignHCenter
          text: qsTr("Individual")
          verticalAlignment: Text.AlignVCenter
        }
      }
      TabButton {
        id: professionalTabButton
        background: Rectangle {
          border.color: InputStyle.highlightColor
          border.width: 1
          color: bar.currentIndex === 1 ? InputStyle.highlightColor : "white"
        }
        contentItem: Text {
          color: bar.currentIndex === 1 ? "white" : InputStyle.highlightColor
          font.pixelSize: InputStyle.fontPixelSizeNormal
          horizontalAlignment: Text.AlignHCenter
          text: qsTr("Professional")
          verticalAlignment: Text.AlignVCenter
        }
      }
    }
    SwipeView {
      id: view
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.top: bar.bottom
      currentIndex: bar.currentIndex
      width: parent.width

      SubscribePlanItem {
        id: individualTab
        name: qsTr("Individual")
        plan: __purchasing.individualPlan

        onSubscribeClicked: {
          __purchasing.purchase(__purchasing.individualPlan.id);
          root.subscribeClicked();
        }
      }
      SubscribePlanItem {
        id: professionalTab
        name: qsTr("Professional")
        plan: __purchasing.professionalPlan

        onSubscribeClicked: {
          __purchasing.purchase(__purchasing.professionalPlan.id);
          root.subscribeClicked();
        }
      }
    }
  }
  TextHyperlink {
    id: textNotice
    anchors.bottom: parent.bottom
    anchors.bottomMargin: InputStyle.rowHeightHeader / 2
    height: InputStyle.rowHeightHeader
    text: qsTr("Your Mergin subscription plan will renew automatically. You can cancel or change it at any time. %1Learn More%2").arg("<a href='" + __inputHelp.merginSubscriptionDetailsLink + "'>").arg("</a>")
    width: parent.width
  }
}
