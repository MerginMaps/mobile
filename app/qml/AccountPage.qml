/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "."  // import InputStyle singleton

Page {
  signal backClicked
  signal managePlansClicked
  signal signOutClicked
  signal restorePurchasesClicked
  property color bgColor: "white"
  property real fieldHeight: InputStyle.rowHeight

  property string username: __merginApi.userAuth.username
  property string email: __merginApi.userInfo.email
  property string planAlias: __merginApi.userInfo.planAlias
  property int storageLimit: __merginApi.userInfo.storageLimit
  property int diskUsage: __merginApi.userInfo.diskUsage
  property int subscriptionStatus: __merginApi.userInfo.subscriptionStatus
  property string subscriptionsTimestamp: __merginApi.userInfo.subscriptionTimestamp
  property string nextBillPrice: __merginApi.userInfo.nextBillPrice
  property bool ownsActiveSubscription: __merginApi.userInfo.ownsActiveSubscription

  id: root
  visible: true


  // /////////////////
  // header
  PanelHeader {
    id: header
    height: InputStyle.rowHeightHeader
    width: parent.width
    color: InputStyle.clrPanelMain
    rowHeight: InputStyle.rowHeightHeader
    titleText: qsTr("Account")
    onBack: backClicked()
    withBackButton: true
  }

  // /////////////////
  // Body
  Column {
    id: accountBodyContainer
    anchors.top: header.bottom
    width: subscribeButton.width
    anchors.horizontalCenter: parent.horizontalCenter

    // avatar
    Row {
        id: avatarContainer
        height: InputStyle.rowHeightHeader * 2
        anchors.horizontalCenter: parent.horizontalCenter

        Item {
          id: avatar
          width: avatarContainer.height
          height: width

          Rectangle {
            id: avatarImage
            anchors.centerIn: parent
            width: avatar.width * 0.8
            height: width
            color: InputStyle.fontColor
            radius: width*0.5
            antialiasing: true

            Image {
              id: userIcon
              anchors.centerIn: parent
              source: 'account.svg'
              height: parent.height * 0.8
              width: height
              sourceSize.width: width
              sourceSize.height: height
              fillMode: Image.PreserveAspectFit
            }

            ColorOverlay {
              anchors.fill: userIcon
              source: userIcon
              color: "#FFFFFF"
            }
          }
        }
    }

    TextWithIcon {
      width: parent.width
      source: 'account.svg'
      text: root.username
    }

    TextWithIcon {
      width: parent.width
      source: 'envelope-solid.svg'
      text: root.email
    }

    TextWithIcon {
      width: parent.width
      source: 'edit.svg'
      text: root.planAlias
    }

    TextWithIcon {
      visible: root.subscriptionStatus === MerginSubscriptionStatus.SubscriptionUnsubscribed
      width: parent.width
      source: 'info.svg'
      text: "<style>a:link { color: " + InputStyle.highlightColor
            + "; text-decoration: underline; }</style>" + qsTr(
              "Your subscription will not be extended after %1")
            .arg(root.subscriptionsTimestamp)
    }

    TextWithIcon {
      visible: root.subscriptionStatus === MerginSubscriptionStatus.SubscriptionInGracePeriod
      width: parent.width
      source: 'exclamation-triangle-solid.svg'
      onLinkActivated: Qt.openUrlExternally(link)
      text: "<style>a:link { color: " + InputStyle.highlightColor
            + "; text-decoration: underline; }</style>" + qsTr(
              "Please fix your %1billing details%2 as soon as possible")
              .arg("<a href='" + __purchasing.subscriptionBillingUrl + "'>")
              .arg("</a>")
      iconColor: InputStyle.highlightColor
    }

    TextWithIcon {
      visible: root.subscriptionStatus === MerginSubscriptionStatus.ValidSubscription
      width: parent.width
      source: 'ic_today.svg'
      text: "<style>a:link { color: " + InputStyle.highlightColor
            + "; text-decoration: underline; }</style>" + qsTr(
              "Your next bill is for %1 on %2")
      .arg(root.nextBillPrice)
      .arg(root.subscriptionsTimestamp)
    }

    TextWithIcon {
      visible: root.subscriptionStatus === MerginSubscriptionStatus.CanceledSubscription
      width: parent.width
      source: 'ic_today.svg'
      text: "<style>a:link { color: " + InputStyle.highlightColor
              + "; text-decoration: underline; }</style>" + qsTr(
                "Your subscription was cancelled on %1")
            .arg(root.subscriptionsTimestamp)
    }

    Row {
      width: parent.width
      Item {
        width: root.fieldHeight
        height: root.fieldHeight

        CircularProgressBar {
          id: storageIcon
          width: parent.width*0.6
          anchors.centerIn: parent
          height: width
          value: root.diskUsage/root.storageLimit
        }
      }

      Text {
        id: textItem
        height: root.fieldHeight
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor
        text: qsTr("Using %1/%2").arg(__inputUtils.bytesToHumanSize(root.diskUsage)).arg(__inputUtils.bytesToHumanSize(root.storageLimit))
      }
    }

    Item {
      //spacer
      height: 10 * InputStyle.dp
      width: parent.width
    }

    Button {
      id: subscribeButton
      enabled: !__purchasing.transactionPending
      width: root.width - 2 * InputStyle.rowHeightHeader
      anchors.horizontalCenter: parent.horizontalCenter
      visible: __merginApi.apiSupportsSubscriptions

      height: InputStyle.rowHeightHeader
      text: __purchasing.transactionPending ? qsTr("Transaction Pending...") : root.ownsActiveSubscription ? qsTr("Manage Subscription") : qsTr("Buy Subscription")
      font.pixelSize: subscribeButton.height / 2

      background: Rectangle {
        color: InputStyle.highlightColor
      }

      onClicked: managePlansClicked()

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

    Item {
      id: spacer
      visible: textRestore.visible
      height: InputStyle.fontPixelSizeTitle
      width:parent.width
    }

    Text {
      id: textRestore
      visible: __merginApi.apiSupportsSubscriptions && __purchasing.hasInAppPurchases && !__purchasing.transactionPending
      textFormat: Text.RichText
      onLinkActivated: restorePurchasesClicked()
      elide: Text.ElideRight
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      text: "<style>a:link { color: " + InputStyle.highlightColor
            + "; text-decoration: underline; }</style>" + qsTr(
              "You can also %1restore%2 your purchases")
            .arg("<a href='http://restore-purchases'>")
            .arg("</a>")
      font.pixelSize: InputStyle.fontPixelSizeNormal
      color: InputStyle.fontColor
      width: root.width - 2 * InputStyle.rowHeightHeader
    }
  }

  // /////////////////
  // Footer
  Column {
    anchors.bottom: parent.bottom
    anchors.horizontalCenter: parent.horizontalCenter

    Button {
      id: signOutButton
      height: InputStyle.fontPixelSizeTitle
      text: qsTr("Sign out")
      font.pixelSize: signOutButton.height
      font.bold: true
      anchors.horizontalCenter: parent.horizontalCenter
      background: Rectangle {
        color: root.bgColor
      }

      onClicked: signOutClicked()

      contentItem: Text {
        text: signOutButton.text
        font: signOutButton.font
        color: InputStyle.highlightColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
      }
    }

    Item {
      id: spacer2
      height: InputStyle.fontPixelSizeTitle
      width:parent.width
    }
  }
}

