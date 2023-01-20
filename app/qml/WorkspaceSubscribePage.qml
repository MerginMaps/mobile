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

import "."  // import InputStyle singleton
import "./components" as MMComponents

Page {
  id: root

  signal back

  header: MMComponents.PanelHeaderV2 {
    width: parent.width
    headerTitle: qsTr("Subscribe")
    onBackClicked: root.back()
  }

  ScrollView {

    anchors {
      fill: parent
    }

    contentWidth: availableWidth // only scroll vertically

    ColumnLayout {

      anchors {
        fill: parent
        leftMargin: InputStyle.outerFieldMargin
        rightMargin: InputStyle.outerFieldMargin
      }

      spacing: InputStyle.formSpacing

      TabBar {
        id: bar

        Layout.fillWidth: true
        Layout.topMargin: InputStyle.smallGap
        Layout.preferredHeight: InputStyle.rowHeightMedium

        spacing: InputStyle.panelSpacing

        TabButton {
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
            radius: InputStyle.cornerRadius
          }
        }

        TabButton {
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
            radius: InputStyle.cornerRadius
          }
        }
      }

      SwipeView {
        currentIndex: bar.currentIndex

        Layout.fillWidth: true
        Layout.preferredHeight: plan.height

        interactive: false
        spacing: InputStyle.formSpacing

        WorkspaceSubscribePlanItem {
          id: plan
          plan: __purchasing.individualPlan

          onSubscribeClicked: {
            __purchasing.purchase( __purchasing.individualPlan.id )
            root.back()
          }
        }

        WorkspaceSubscribePlanItem {
          plan: __purchasing.professionalPlan

          onSubscribeClicked: {
            __purchasing.purchase( __purchasing.professionalPlan.id )
            root.back()
          }
        }
      }

      Text {
        Layout.fillWidth: true
        Layout.preferredHeight: InputStyle.rowHeightHeader

        color: InputStyle.fontColor
        linkColor: InputStyle.highlightColor

        wrapMode: Text.WordWrap
        textFormat: Text.StyledText

        font.bold: true
        font.pixelSize: InputStyle.fontPixelSizeNormal

        verticalAlignment: Qt.AlignVCenter
        horizontalAlignment: Qt.AlignHCenter

        visible: !__purchasing.transactionPending

        text: qsTr("You can also %1restore%2 your purchases.")
          .arg("<a href='http://restore-purchases'>")
          .arg("</a>")

        onLinkActivated: __purchasing.restore()
      }

      MMComponents.TextHyperlink {
        Layout.fillWidth: true
        Layout.preferredHeight: InputStyle.rowHeightHeader
        Layout.bottomMargin: InputStyle.smallGap

        text: qsTr("Your Mergin subscription plan will renew automatically. You can cancel or change it at any time. %1Learn More%2")
          .arg("<a href='" + __inputHelp.merginSubscriptionDetailsLink + "'>")
          .arg("</a>")
      }
    }
  }
}
