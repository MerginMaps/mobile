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
import "./components" as MMComponents

Page {
  id: root

  signal back
  signal signOutClicked
  signal managePlansClicked
  signal accountDeleted
  signal switchWorkspace

  // user-related
  property string email: __merginApi.userInfo.email
  property string fullname: __merginApi.userInfo.name
  property string username: __merginApi.userAuth.username

  // workspace-related
  property string role: __merginApi.workspaceInfo.role
  property int diskUsage: __merginApi.workspaceInfo.diskUsage
  property int storageLimit: __merginApi.workspaceInfo.storageLimit
  property string workspaceName: __merginApi.userInfo.activeWorkspaceName

  // (ws) subscription-related
  property string planAlias: __merginApi.subscriptionInfo.planAlias
  property int subscriptionStatus: __merginApi.subscriptionInfo.subscriptionStatus
  property string subscriptionsTimestamp: __merginApi.subscriptionInfo.subscriptionTimestamp
  property string nextBillPrice: __merginApi.subscriptionInfo.nextBillPrice
  property bool ownsActiveSubscription: __merginApi.subscriptionInfo.ownsActiveSubscription
  property bool canAccessSubscription: __merginApi.subscriptionInfo.canAccessSubscription

  property bool apiSupportsSubscriptions: __merginApi.apiSupportsSubscriptions

  header: MMComponents.PanelHeaderV2 {
    width: root.width
    headerTitle: qsTr( "My Account" )
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

      Item {
        id: avatar

        Layout.fillWidth: true
        Layout.preferredHeight: InputStyle.iconSizeXXLarge

        Rectangle {
          height: avatar.height
          width: height
          anchors.centerIn: parent

          radius: width / 2
          antialiasing: true
          color: InputStyle.clrPanelBackground

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
            color: InputStyle.panelBackgroundWhite
          }
        }
      }

      RowLayout {

        Layout.fillWidth: true
        Layout.preferredHeight: InputStyle.rowHeight

        spacing: InputStyle.formSpacing

        MMComponents.TextRowWithTitle {
          Layout.fillWidth: true

          textComponent.font.bold: true
          textComponent.wrapMode: Text.NoWrap
          textComponent.elide: Text.ElideMiddle

          titleText: qsTr( "Workspace" )
          text: root.workspaceName
        }

        MMComponents.DelegateButton {

          Layout.preferredWidth: root.width / 3
          btnWidth: width
          btnHeight: InputStyle.mediumBtnHeight
          height: InputStyle.rowHeightMedium
          fontPixelSize: InputStyle.fontPixelSizeSmall
          visible: __merginApi.apiSupportsWorkspaces

          text: qsTr("Switch workspace")

          onClicked: root.switchWorkspace()
        }
      }

      MMComponents.TextRowWithTitle {
        Layout.fillWidth: true

        titleText: qsTr( "Your role" )
        text: root.role
      }

      MMComponents.TextRowWithTitle {
        Layout.fillWidth: true

        titleText: qsTr( "Workspace storage" )
        text: qsTr("Using %1 / %2").arg(__inputUtils.bytesToHumanSize(root.diskUsage)).arg(__inputUtils.bytesToHumanSize(root.storageLimit))
      }

      MMComponents.TextRowWithTitle {
        Layout.fillWidth: true

        visible: root.apiSupportsSubscriptions && root.canAccessSubscription

        titleText: qsTr( "Subscription plan" )
        text: root.planAlias
      }

      MMComponents.TextRowWithTitle {
        Layout.fillWidth: true

        visible: root.apiSupportsSubscriptions && root.canAccessSubscription && root.subscriptionStatus === MerginSubscriptionStatus.SubscriptionUnsubscribed

        titleText: qsTr( "Subscription status" )
        text: qsTr("Your subscription will not auto-renew after %1").arg(root.subscriptionsTimestamp)
      }

      MMComponents.TextRowWithTitle {
        Layout.fillWidth: true

        visible: root.apiSupportsSubscriptions && root.canAccessSubscription && root.subscriptionStatus === MerginSubscriptionStatus.SubscriptionInGracePeriod

        titleText: qsTr( "Subscription status" )
        text: qsTr("Please update your %1billing details%2 as soon as possible")
                .arg("<a href='" + __purchasing.subscriptionBillingUrl + "'>")
                .arg("</a>")

        textComponent.onLinkActivated: function ( link ) {
          Qt.openUrlExternally( link )
        }
      }

      MMComponents.TextRowWithTitle {
        Layout.fillWidth: true

        visible: root.apiSupportsSubscriptions && root.canAccessSubscription && root.subscriptionStatus === MerginSubscriptionStatus.ValidSubscription

        titleText: qsTr( "Subscription status" )
        text: qsTr("Your next bill will be for %1 on %2")
                .arg(root.nextBillPrice)
                .arg(root.subscriptionsTimestamp)
      }

      MMComponents.TextRowWithTitle {
        Layout.fillWidth: true

        visible: root.apiSupportsSubscriptions && root.canAccessSubscription && root.subscriptionStatus === MerginSubscriptionStatus.CanceledSubscription

        titleText: qsTr( "Subscription status" )
        text: qsTr("Your subscription was cancelled on %1").arg(root.subscriptionsTimestamp)
      }

      MMComponents.DelegateButton {

        Layout.fillWidth: true
        btnWidth: width
        height: InputStyle.rowHeightMedium

        visible: root.apiSupportsSubscriptions && root.canAccessSubscription

        text: qsTr("Subscriptions")

        onClicked: root.managePlansClicked()
      }

      // user profile

      Rectangle {
        Layout.preferredHeight: InputStyle.borderSize
        Layout.fillWidth: true
        color: InputStyle.panelBackgroundLight
      }

      MMComponents.TextRowWithTitle {
        Layout.fillWidth: true
        Layout.topMargin: InputStyle.smallGap

        textComponent.font.bold: true

        titleText: qsTr( "Account" )
        text: root.fullname ? root.fullname : root.username
      }

      MMComponents.TextRowWithTitle {
        Layout.fillWidth: true

        visible: root.fullname // show only if user has specified their full name

        titleText: qsTr( "Username" )
        text: root.username
      }

      MMComponents.TextRowWithTitle {
        Layout.fillWidth: true

        titleText: qsTr( "Email" )
        text: root.email
      }

      MMComponents.DelegateButton {

        Layout.fillWidth: true
        btnWidth: width
        height: InputStyle.rowHeightMedium

        text: qsTr("Sign out")

        onClicked: root.signOutClicked()
      }

      Label {
        text: qsTr( "Delete account" )

        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.invalidButtonColor

        Layout.fillWidth: true
        Layout.leftMargin: InputStyle.formSpacing
        Layout.topMargin: InputStyle.smallGap
        Layout.bottomMargin: InputStyle.smallGap

        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        MouseArea {
          anchors.fill: parent
          onClicked: accountDeleteDialog.open()
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

      visible: false
      title: qsTr( "Failed to remove account" )
      text: qsTr( "An error occured while removing your account" )
      buttons: MessageDialog.Close

      onButtonClicked: function( clickedButton ) {
        accountDeleteIndicator.running = false
        close()
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
        accountDeletionFailedDialog.text = qsTr("Can not close account because user is the only owner of an organisation.\n\n" +
                                                       "Please go to the Mergin Maps website to remove it manually.")
        accountDeletionFailedDialog.open()
      }
      function onAccountDeleted( result ) {
        accountDeleteIndicator.running = false
        if ( result ) {
            accountDeleted()
        }
        else {
          accountDeletionFailedDialog.open()
        }
      }
    }

}
