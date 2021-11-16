/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.12
import "components"

Dialog {
  id: root
  property int diskUsage: __merginApi.userInfo.diskUsage
  property real fieldHeight: InputStyle.rowHeight
  property string planAlias: __merginApi.subscriptionInfo.planAlias
  property int storageLimit: __merginApi.userInfo.storageLimit
  property string titleText: qsTr("You have reached a data limit")
  property var uploadSize

  anchors.centerIn: parent
  closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
  margins: InputStyle.outerFieldMargin
  modal: true
  visible: false

  signal openSubscriptionPlans

  onAccepted: root.close()

  background: Rectangle {
    anchors.fill: parent
    color: "white"
  }
  contentItem: ColumnLayout {
    anchors.margins: InputStyle.outerFieldMargin

    Text {
      id: title
      anchors.topMargin: InputStyle.panelMargin
      color: InputStyle.fontColor
      font.bold: true
      font.pixelSize: InputStyle.fontPixelSizeTitle
      height: root.fieldHeight
      horizontalAlignment: Text.AlignHCenter
      text: root.titleText
      verticalAlignment: Text.AlignVCenter
    }
    TextWithIcon {
      height: root.fieldHeight
      source: InputStyle.syncIcon
      text: qsTr("Data to sync: %1").arg(__inputUtils.bytesToHumanSize(uploadSize))
      width: parent.width
    }
    Row {
      height: root.fieldHeight
      width: parent.width

      Item {
        height: root.fieldHeight
        width: root.fieldHeight

        CircularProgressBar {
          id: storageIcon
          anchors.centerIn: parent
          height: width
          value: root.diskUsage / root.storageLimit
          width: parent.width * 0.6
        }
      }
      Text {
        id: textItem
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeNormal
        height: root.fieldHeight
        text: qsTr("Using %1 / %2").arg(__inputUtils.bytesToHumanSize(diskUsage)).arg(__inputUtils.bytesToHumanSize(storageLimit))
        verticalAlignment: Text.AlignVCenter
      }
    }
    TextWithIcon {
      height: root.fieldHeight
      source: InputStyle.editIcon
      text: qsTr("Plan: %1").arg(planAlias)
      visible: __merginApi.apiSupportsSubscriptions
      width: parent.width
    }
    Text {
      id: planLink
      Layout.fillWidth: true
      color: InputStyle.fontColor
      font.pixelSize: InputStyle.fontPixelSizeNormal
      font.underline: true
      height: root.fieldHeight
      horizontalAlignment: Qt.AlignHCenter
      text: qsTr("Manage subscriptions")
      verticalAlignment: Qt.AlignVCenter
      visible: __merginApi.apiSupportsSubscriptions

      MouseArea {
        anchors.fill: parent

        onClicked: root.openSubscriptionPlans()
      }
    }
    Item {
      id: spacer
      height: InputStyle.fontPixelSizeTitle
      width: parent.width
    }
    DelegateButton {
      Layout.fillWidth: true
      height: root.fieldHeight
      text: qsTr("Cancel")
      width: parent.width

      onClicked: root.close()
    }
  }
}
