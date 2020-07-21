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

Rectangle {
  id: root
  visible: true

  signal backClicked
  signal subscribeClicked

  property int sp: 10 * InputStyle.dp

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

  Column {
    id: subscribeBodyContainer
    anchors.top: header.bottom
    width: subscribeButton.width
    anchors.horizontalCenter: parent.horizontalCenter
    spacing: 5

    Image {
      id: merginLogo
      antialiasing: true
      source: 'mergin_color.svg'
      height: InputStyle.rowHeightHeader
      width: parent.width
      sourceSize.width: width
      sourceSize.height: height
      fillMode: Image.PreserveAspectFit
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignHCenter
    }

    Text {
      id: textCurrent
      text: ""
      font.pixelSize: InputStyle.fontPixelSizeNormal
      color: InputStyle.fontColor
      textFormat: Text.RichText
    }


    TextWithIcon {
      width: parent.width
      source: 'ic_today.svg'
      text: __merginApi.userInfo.ownsActiveSubscription ? qsTr("Custom billing period") : __purchasing.recommendedPlan.period
    }

    TextWithIcon {
      width: parent.width
      source: 'database-solid.svg'
      text: __merginApi.userInfo.ownsActiveSubscription ? qsTr("Custom storage") : __purchasing.recommendedPlan.storage
    }

    TextWithIcon {
      width: parent.width
      source: 'account-multi.svg'
      text: "Unlimited collaborators"
    }

    TextWithIcon {
      width: parent.width
      source: 'project.svg'
      text: "Unlimited projects"
    }

    TextWithIcon {
      width: parent.width
      source: 'envelope-solid.svg'
      text: "Email support"
    }

    Button {
      id: subscribeButton
      width: root.width - 2 * InputStyle.rowHeightHeader
      anchors.horizontalCenter: parent.horizontalCenter

      height: InputStyle.rowHeightHeader
      text: __merginApi.userInfo.ownsActiveSubscription ? qsTr("Manage") : __purchasing.recommendedPlan.price
      font.pixelSize: subscribeButton.height / 2

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
    Text {
      id: textNotice
      anchors.bottom: parent.bottom
      textFormat: Text.RichText
      onLinkActivated: Qt.openUrlExternally(link)
      elide: Text.ElideRight
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      text: "<style>a:link { color: " + InputStyle.highlightColor
            + "; text-decoration: underline; }</style>" + qsTr(
              "Your Mergin storage plan will automatically renew.<br>You can cancel or change at any time. <a href='%1'>Learn More</a>").arg(__purchasing.subscriptionManageUrl)
      font.pixelSize: InputStyle.fontPixelSizeNormal
      color: InputStyle.fontColor
      width: parent.width
    }
}
