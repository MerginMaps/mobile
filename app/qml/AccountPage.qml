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
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import QtQuick.Dialogs
import lc 1.0
import "."  // import InputStyle singleton
import "./components"

Page {
  signal back
  signal managePlansClicked
  signal signOutClicked
  signal restorePurchasesClicked
  signal accountDeleted
  property color bgColor: "white"
  property real fieldHeight: InputStyle.rowHeight

  property string username: __merginApi.userAuth.username
  property string email: __merginApi.userInfo.email
  property int diskUsage: __merginApi.userInfo.diskUsage
  property int storageLimit: __merginApi.userInfo.storageLimit
  property string planAlias: __merginApi.subscriptionInfo.planAlias
  property int subscriptionStatus: __merginApi.subscriptionInfo.subscriptionStatus
  property string subscriptionsTimestamp: __merginApi.subscriptionInfo.subscriptionTimestamp
  property string nextBillPrice: __merginApi.subscriptionInfo.nextBillPrice
  property bool ownsActiveSubscription: __merginApi.subscriptionInfo.ownsActiveSubscription
  property bool apiSupportsSubscriptions: __merginApi.apiSupportsSubscriptions

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
    titleText: qsTr("My Account")
    onBack: root.back()
    withBackButton: true
  }


  ScrollView {
    id: scrollView
    anchors.top: header.bottom
    width: root.width
    height: root.height - header.height
    contentHeight: accountBodyContainer.height + footer.height
    clip: true

    // /////////////////
    // Body
    Column {
      id: accountBodyContainer
      width: root.width
      height: Math.max(accountBodyContainer.childrenRect.height, scrollView.height - footer.height)

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
                source: InputStyle.accountIcon
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
        height: InputStyle.rowHeight
        source: InputStyle.accountIcon
        text: root.username
      }

      TextWithIcon {
        width: parent.width
        height: InputStyle.rowHeight
        source: InputStyle.envelopeIcon
        text: root.email
      }

      TextWithIcon {
        width: parent.width
        height: InputStyle.rowHeight
        visible: root.apiSupportsSubscriptions
        source: InputStyle.editIcon
        text: root.planAlias
      }

      TextWithIcon {
        width: parent.width
        height: InputStyle.rowHeight
        visible: root.subscriptionStatus === MerginSubscriptionStatus.SubscriptionUnsubscribed
        source: InputStyle.infoIcon
        text: qsTr("Your subscription will not auto-renew after %1")
              .arg(root.subscriptionsTimestamp)
      }

      TextWithIcon {
        width: parent.width
        height: InputStyle.rowHeight
        visible: root.subscriptionStatus === MerginSubscriptionStatus.SubscriptionInGracePeriod
        source: InputStyle.exclamationTriangleIcon
        onLinkActivated: function( link ) {
          Qt.openUrlExternally(link)
        }
        text: qsTr("Please update your %1billing details%2 as soon as possible")
                .arg("<a href='" + __purchasing.subscriptionBillingUrl + "'>")
                .arg("</a>")
        iconColor: InputStyle.highlightColor
      }

      TextWithIcon {
        width: parent.width
        height: InputStyle.rowHeight
        visible: root.subscriptionStatus === MerginSubscriptionStatus.ValidSubscription
        source: InputStyle.todayIcon
        text: qsTr("Your next bill will be for %1 on %2")
        .arg(root.nextBillPrice)
        .arg(root.subscriptionsTimestamp)
      }

      TextWithIcon {
        width: parent.width
        height: InputStyle.rowHeight
        visible: root.subscriptionStatus === MerginSubscriptionStatus.CanceledSubscription
        source: InputStyle.todayIcon
        text: qsTr("Your subscription was cancelled on %1")
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
          text: qsTr("Using %1 / %2").arg(__inputUtils.bytesToHumanSize(root.diskUsage)).arg(__inputUtils.bytesToHumanSize(root.storageLimit))
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
        text: __purchasing.transactionPending ? qsTr("Working...") : root.ownsActiveSubscription ? qsTr("Manage Subscription") : qsTr("Subscription plans")
        font.pixelSize: InputStyle.fontPixelSizeBig

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
        height: InputStyle.rowHeightHeader
        width:parent.width
      }

      Text {
        id: textRestore
        visible: __iosUtils.isIos && __merginApi.apiSupportsSubscriptions && __purchasing.hasInAppPurchases && !__purchasing.transactionPending
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
        width: root.width
        leftPadding: InputStyle.rowHeightHeader
        rightPadding: InputStyle.rowHeightHeader
      }
    }

    // /////////////////
    // Footer
    Item {
      id: footer
      height: InputStyle.rowHeight * 2
      width: parent.width
      anchors.bottom: parent.bottom
      anchors.horizontalCenter: parent.horizontalCenter

      Column {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        spacing: 5

        Button {
          id: signOutButton
          height: InputStyle.rowHeightHeader
          text: qsTr("Sign out")
          font.pixelSize: InputStyle.fontPixelSizeNormal
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

        Button {
          id: deleteAccountButton
          height: InputStyle.rowHeightHeader
          text: qsTr("Delete account")
          font.pixelSize: InputStyle.fontPixelSizeNormal
          font.bold: true
          anchors.horizontalCenter: parent.horizontalCenter
          background: Rectangle {
            color: root.bgColor
          }

          onClicked: accountDeleteDialog.open()

          contentItem: Text {
            text: deleteAccountButton.text
            font: deleteAccountButton.font
            color: InputStyle.invalidButtonColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
          }
        }

      }
    }

    Dialog {
      id: accountDeleteDialog
      visible: false
      modal: true
      spacing: InputStyle.panelSpacing
      anchors.centerIn: parent
      leftMargin: InputStyle.panelMargin
      rightMargin: InputStyle.panelMargin
      title: qsTr( "Delete account?" )

      contentItem: ColumnLayout {
        id: column
        Label {
          id: label
          text: qsTr("This action will delete your Mergin Maps account with all your projects, " +
                     "both on the device and on the server. This action cannot be undone. " +
                     "If you have an Apple subscription you need to cancel it manually.\n\n" +
                     "In order to delete your account, enter your username in the field below and click Yes.")
          Layout.fillWidth: true
          wrapMode: Text.WordWrap
        }
        TextField {
          id: usernameField
          placeholderText: qsTr("Enter username")
          Layout.fillWidth: true
          onTextEdited: function() {
            buttons.standardButton(Dialog.Yes).enabled = (text === username)
          }
        }
      }

      footer: DialogButtonBox {
        id: buttons
        standardButtons: Dialog.Yes | Dialog.No
      }

      onAboutToShow: {
        buttons.standardButton(Dialog.Yes).enabled = false;
      }

      onAboutToHide: {
        usernameField.clear()
      }

      onAccepted: {
         close()
         accountDeleteIndicator.running = true
         __merginApi.deleteAccount()
      }
      onRejected: {
        close()
      }
    }

    MessageDialog {
      id: accountDeletionFailedDialog

      property string messageText

      visible: false
      title: qsTr( "Failed to remove account" )
      text: messageText
      buttons: MessageDialog.Close
      onRejected: {
        close()
        accountDeleted()
      }
    }

    BusyIndicator {
      id: accountDeleteIndicator
      width: root.width/8
      height: width
      running: false
      visible: running
      anchors.centerIn: parent
      z: root.z + 1
    }

    Connections {
      target: __merginApi

      function onUserIsAnOrgOwnerError() {
        accountDeleteIndicator.running = false
        accountDeletionFailedDialog.messageText = qsTr("Can not close account because user is the only owner of organisation.\n\n" +
                                                       "Please go to the Mergin Maps <a href='%1'>dashboard</a> to remove it manually.".arg(__merginApi.apiRoot()))
        accountDeletionFailedDialog.open()
      }
      function onAccountDeleted( result ) {
        accountDeleteIndicator.running = false
        if ( result ) {
            accountDeleted()
        }
        else {
          accountDeletionFailedDialog.messageText = qsTr( "Failed to remove account" )
          accountDeletionFailedDialog.open()
        }
      }
    }
  }
}
