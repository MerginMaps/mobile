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
import QtQuick.Dialogs
import QtQuick.Layouts

import "./components"

Dialog {
  property int maxProjectNumber
  property string titleText: qsTr("You have reached project number limit")
  property int diskUsage: __merginApi.workspaceInfo.diskUsage
  property int storageLimit: __merginApi.workspaceInfo.storageLimit
  property string planAlias: __merginApi.subscriptionInfo.planAlias
  property real fieldHeight: InputStyle.rowHeight

  signal openSubscriptionPlans

  id: root
  visible: false
  modal: true
  closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
  onAccepted: root.close()
  anchors.centerIn: parent
  margins: InputStyle.outerFieldMargin

  background: Rectangle {
    anchors.fill: parent
    color: "white"
  }

  contentItem: ColumnLayout {
    anchors.margins: InputStyle.outerFieldMargin

    Text {
      id: title
      anchors.topMargin: InputStyle.panelMargin
      height: root.fieldHeight
      text: root.titleText
      color: InputStyle.fontColor
      font.pixelSize: InputStyle.fontPixelSizeBig
      font.bold: true
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignHCenter
    }

    TextWithIcon {
      width: parent.width
      height: root.fieldHeight
      source: InputStyle.projectIcon
      text: qsTr("Maximum of projects: %1").arg(maxProjectNumber)
    }

    TextWithIcon {
      width: parent.width
      height: root.fieldHeight
      source: InputStyle.editIcon
      text: qsTr("Plan: %1").arg(planAlias)
      visible: __merginApi.apiSupportsSubscriptions
    }

    Text {
      id: planLink
      height: root.fieldHeight
      Layout.fillWidth: true
      horizontalAlignment: Qt.AlignHCenter
      verticalAlignment: Qt.AlignVCenter
      font.pixelSize: InputStyle.fontPixelSizeNormal
      font.underline: true
      color: InputStyle.fontColor
      text: qsTr("Manage account")
      visible: __merginApi.apiSupportsSubscriptions

      MouseArea {
        anchors.fill: parent
        onClicked: root.openSubscriptionPlans()
      }
    }

    Item {
      id: spacer
      height: InputStyle.smallGap
      width: parent.width
    }

    DelegateButton {
      text: qsTr("Cancel")
      onClicked: root.close()
      height: root.fieldHeight
      width: parent.width
      Layout.fillWidth: true
    }
  }
}
