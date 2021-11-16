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

Item {
  id: root
  property bool hasPlan: __merginApi.subscriptionInfo.ownsActiveSubscription || !root.plan
  property string name
  property var plan

  visible: true

  signal subscribeClicked

  Column {
    leftPadding: InputStyle.rowHeightHeader
    rightPadding: InputStyle.rowHeightHeader
    spacing: 5
    width: parent.width

    TextWithIcon {
      source: InputStyle.infoIcon
      text: "Mergin " + root.name + " " + qsTr("Plan")
      width: parent.width
    }
    TextWithIcon {
      source: InputStyle.todayIcon
      text: hasPlan ? "Custom billing period" : root.plan.period /* Do not translate, only used for test subscriptions */
      width: parent.width
    }
    TextWithIcon {
      source: InputStyle.databaseIcon
      text: hasPlan ? "Custom storage" : root.plan.storage /* Do not translate, only used for test subscriptions */
      width: parent.width
    }
    TextWithIcon {
      source: InputStyle.accountMultiIcon
      text: qsTr("Unlimited collaborators")
      width: parent.width
    }
    TextWithIcon {
      source: InputStyle.projectIcon
      text: qsTr("Unlimited projects")
      width: parent.width
    }
    TextWithIcon {
      source: InputStyle.envelopeIcon
      text: qsTr("Email support")
      width: parent.width
    }
    Button {
      id: subscribeButton
      anchors.horizontalCenter: parent.horizontalCenter
      enabled: text !== ''
      font.pixelSize: InputStyle.fontPixelSizeTitle
      height: InputStyle.rowHeightHeader
      text: hasPlan ? "Manage" : root.plan.price /* Do not translate, only used for test subscriptions */
      width: root.width - 2 * InputStyle.rowHeightHeader

      onClicked: subscribeClicked()

      background: Rectangle {
        color: InputStyle.highlightColor
      }
      contentItem: Text {
        color: "white"
        elide: Text.ElideRight
        font: subscribeButton.font
        horizontalAlignment: Text.AlignHCenter
        opacity: enabled ? 1.0 : 0.3
        text: subscribeButton.text
        verticalAlignment: Text.AlignVCenter
      }
    }
  }
}
