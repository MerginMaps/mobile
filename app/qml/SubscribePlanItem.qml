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

Item {
  id: root
  visible: true

  signal subscribeClicked


  property var plan
  property string name

  property bool hasPlan: __merginApi.subscriptionInfo.ownsActiveSubscription || !root.plan

  Column {
    width: parent.width
    spacing: 5
    leftPadding: InputStyle.rowHeightHeader
    rightPadding: InputStyle.rowHeightHeader

    TextWithIcon {
      width: parent.width
      height: InputStyle.rowHeight
      source: InputStyle.infoIcon
      text: "Mergin " + root.name + " " + qsTr("Plan")
    }

    TextWithIcon {
      width: parent.width
      height: InputStyle.rowHeight
      source: InputStyle.todayIcon
      text: hasPlan ? "Custom billing period" : root.plan.period /* Do not translate, only used for test subscriptions */
    }

    TextWithIcon {
      width: parent.width
      height: InputStyle.rowHeight
      source: InputStyle.databaseIcon
      text: hasPlan ? "Custom storage" : root.plan.storage /* Do not translate, only used for test subscriptions */
    }

    TextWithIcon {
      width: parent.width
      height: InputStyle.rowHeight
      source: InputStyle.accountMultiIcon
      text: qsTr("Unlimited collaborators")
    }

    TextWithIcon {
      width: parent.width
      height: InputStyle.rowHeight
      source: InputStyle.projectIcon
      text: qsTr("Unlimited projects")
    }

    TextWithIcon {
      width: parent.width
      height: InputStyle.rowHeight
      source: InputStyle.envelopeIcon
      text: qsTr("Email support")
    }

    Button {
      id: subscribeButton
      width: root.width - 2 * InputStyle.rowHeightHeader
      anchors.horizontalCenter: parent.horizontalCenter

      height: InputStyle.rowHeightHeader
      text: hasPlan ? "Manage" : root.plan.price /* Do not translate, only used for test subscriptions */
      enabled: text !== ''
      font.pixelSize: InputStyle.fontPixelSizeBig

      background: Rectangle {
        color: InputStyle.highlightColor
      }

      onClicked: subscribeClicked()

      contentItem: Text {
        text: subscribeButton.text
        font: subscribeButton.font
        opacity: enabled ? 1.0 : 0.3
        color: "white"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
      }

    }
  }

}
