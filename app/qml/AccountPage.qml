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
import "components"

Page {
  id: root
  property bool apiSupportsSubscriptions: __merginApi.apiSupportsSubscriptions
  property color bgColor: "white"
  property int diskUsage: __merginApi.userInfo.diskUsage
  property string email: __merginApi.userInfo.email
  property real fieldHeight: InputStyle.rowHeight
  property string nextBillPrice: __merginApi.subscriptionInfo.nextBillPrice
  property bool ownsActiveSubscription: __merginApi.subscriptionInfo.ownsActiveSubscription
  property string planAlias: __merginApi.subscriptionInfo.planAlias
  property int storageLimit: __merginApi.userInfo.storageLimit
  property int subscriptionStatus: __merginApi.subscriptionInfo.subscriptionStatus
  property string subscriptionsTimestamp: __merginApi.subscriptionInfo.subscriptionTimestamp
  property string username: __merginApi.userAuth.username

  visible: true

  signal back
  signal managePlansClicked
  signal restorePurchasesClicked
  signal signOutClicked

  // /////////////////
  // header
  PanelHeader {
    id: header
    color: InputStyle.clrPanelMain
    height: InputStyle.rowHeightHeader
    rowHeight: InputStyle.rowHeightHeader
    titleText: qsTr("My Account")
    width: parent.width
    withBackButton: true

    onBack: root.back()
  }
  ScrollView {
    id: scrollView
    anchors.top: header.bottom
    clip: true
    contentHeight: accountBodyContainer.height + footer.height
    height: root.height - header.height
    width: root.width

    // /////////////////
    // Body
    Column {
      id: accountBodyContainer
      height: Math.max(accountBodyContainer.childrenRect.height, scrollView.height - footer.height)
      width: root.width

      // avatar
      Row {
        id: avatarContainer
        anchors.horizontalCenter: parent.horizontalCenter
        height: InputStyle.rowHeightHeader * 2

        Item {
          id: avatar
          height: width
          width: avatarContainer.height

          Rectangle {
            id: avatarImage
            anchors.centerIn: parent
            antialiasing: true
            color: InputStyle.fontColor
            height: width
            radius: width * 0.5
            width: avatar.width * 0.8

            Image {
              id: userIcon
              anchors.centerIn: parent
              fillMode: Image.PreserveAspectFit
              height: parent.height * 0.8
              source: InputStyle.accountIcon
              sourceSize.height: height
              sourceSize.width: width
              width: height
            }
            ColorOverlay {
              anchors.fill: userIcon
              color: "#FFFFFF"
              source: userIcon
            }
          }
        }
      }
      TextWithIcon {
        source: InputStyle.accountIcon
        text: root.username
        width: parent.width
      }
      TextWithIcon {
        source: InputStyle.envelopeIcon
        text: root.email
        width: parent.width
      }
      TextWithIcon {
        source: InputStyle.editIcon
        text: root.planAlias
        visible: root.apiSupportsSubscriptions
        width: parent.width
      }
      TextWithIcon {
        source: InputStyle.infoIcon
        text: qsTr("Your subscription will not auto-renew after %1").arg(root.subscriptionsTimestamp)
        visible: root.subscriptionStatus === MerginSubscriptionStatus.SubscriptionUnsubscribed
        width: parent.width
      }
      TextWithIcon {
        iconColor: InputStyle.highlightColor
        source: InputStyle.exclamationTriangleIcon
        text: qsTr("Please update your %1billing details%2 as soon as possible").arg("<a href='" + __purchasing.subscriptionBillingUrl + "'>").arg("</a>")
        visible: root.subscriptionStatus === MerginSubscriptionStatus.SubscriptionInGracePeriod
        width: parent.width

        onLinkActivated: Qt.openUrlExternally(link)
      }
      TextWithIcon {
        source: InputStyle.todayIcon
        text: qsTr("Your next bill will be for %1 on %2").arg(root.nextBillPrice).arg(root.subscriptionsTimestamp)
        visible: root.subscriptionStatus === MerginSubscriptionStatus.ValidSubscription
        width: parent.width
      }
      TextWithIcon {
        source: InputStyle.todayIcon
        text: qsTr("Your subscription was cancelled on %1").arg(root.subscriptionsTimestamp)
        visible: root.subscriptionStatus === MerginSubscriptionStatus.CanceledSubscription
        width: parent.width
      }
      Row {
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
          text: qsTr("Using %1 / %2").arg(__inputUtils.bytesToHumanSize(root.diskUsage)).arg(__inputUtils.bytesToHumanSize(root.storageLimit))
          verticalAlignment: Text.AlignVCenter
        }
      }
      Item {
        //spacer
        height: 10 * InputStyle.dp
        width: parent.width
      }
      Button {
        id: subscribeButton
        anchors.horizontalCenter: parent.horizontalCenter
        enabled: !__purchasing.transactionPending
        font.pixelSize: InputStyle.fontPixelSizeTitle
        height: InputStyle.rowHeightHeader
        text: __purchasing.transactionPending ? qsTr("Working...") : root.ownsActiveSubscription ? qsTr("Manage Subscription") : qsTr("Subscription plans")
        visible: __merginApi.apiSupportsSubscriptions
        width: root.width - 2 * InputStyle.rowHeightHeader

        onClicked: managePlansClicked()

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
      Item {
        id: spacer
        height: InputStyle.fontPixelSizeTitle
        visible: textRestore.visible
        width: parent.width
      }
      Text {
        id: textRestore
        color: InputStyle.fontColor
        elide: Text.ElideRight
        font.pixelSize: InputStyle.fontPixelSizeNormal
        horizontalAlignment: Text.AlignHCenter
        leftPadding: InputStyle.rowHeightHeader
        rightPadding: InputStyle.rowHeightHeader
        text: "<style>a:link { color: " + InputStyle.highlightColor + "; text-decoration: underline; }</style>" + qsTr("You can also %1restore%2 your purchases").arg("<a href='http://restore-purchases'>").arg("</a>")
        textFormat: Text.RichText
        verticalAlignment: Text.AlignVCenter
        visible: __iosUtils.isIos && __merginApi.apiSupportsSubscriptions && __purchasing.hasInAppPurchases && !__purchasing.transactionPending
        width: root.width

        onLinkActivated: restorePurchasesClicked()
      }
    }

    // /////////////////
    // Footer
    Item {
      id: footer
      anchors.bottom: parent.bottom
      anchors.horizontalCenter: parent.horizontalCenter
      height: InputStyle.rowHeight
      width: parent.width

      Button {
        id: signOutButton
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.bold: true
        font.pixelSize: signOutButton.height
        height: InputStyle.fontPixelSizeTitle
        text: qsTr("Sign out")

        onClicked: signOutClicked()

        background: Rectangle {
          color: root.bgColor
        }
        contentItem: Text {
          color: InputStyle.highlightColor
          elide: Text.ElideRight
          font: signOutButton.font
          horizontalAlignment: Text.AlignHCenter
          text: signOutButton.text
          verticalAlignment: Text.AlignVCenter
        }
      }
    }
  }
}
