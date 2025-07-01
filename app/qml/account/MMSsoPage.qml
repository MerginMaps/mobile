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

import mm 1.0 as MM

import "./components" as MMAccountComponents
import "../components"
import "../inputs"


MMPage {
  id: root

  signal signInClicked( string email )
  signal loginWithPasswordClicked

  property alias loadingDialog: loadingDialog

  pageHeader {
    title: qsTr( "Sign in with SSO" )
    titleFont: __style.h3
    baseHeaderHeight: __style.row80
  }

  pageBottomMarginPolicy: MMPage.BottomMarginPolicy.PaintBehindSystemBar

  pageContent: MMScrollView {
    id: contentScroller

    width: parent.width
    height: parent.height

    Column {
      id: maincol

      width: parent.width

      spacing: __style.spacing20

      MMListSpacer {
        height: __style.margin20
      }

      MMTextInput {
        id: email

        width: parent.width

        title: qsTr( "Work email" )
        textField.inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhEmailCharactersOnly
        text: __merginApi.userAuth.login
      }

      MMListSpacer {}

      Text {
        text: __inputUtils.htmlLink(
          qsTr("By continuing with SSO, you accept the %1Terms and Conditions%3 and %2Privacy Policy%3"),
          __style.forestColor,
          __inputHelp.merginTermsLink,
          __inputHelp.privacyPolicyLink
        )

        font: __style.p5
        color: __style.nightColor

        textFormat: Text.RichText

        width: parent.width
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        lineHeight: 1.5 // mimic line height with factor
        wrapMode: Text.WordWrap

        onLinkActivated: function ( link ) {
          Qt.openUrlExternally( link )
        }
      }

      MMListSpacer {}

      MMButton {
        width: parent.width

        text: qsTr( "Sign in" )

        enabled: __inputUtils.isValidEmail( email.text )

        onClicked: {
          root.signInClicked( email.text )
        }
      }

      MMListSpacer { height: __style.margin20 }

      MMHlineText {
        width: parent.width

        title: qsTr("Use password instead?")
      }

      MMButton {
        width: parent.width

        text: qsTr( "Sign in with password" )

        type: MMButton.Types.Secondary

        onClicked: root.loginWithPasswordClicked()
      }

      MMListFooterSpacer {}
    }
  }

  MMDrawerDialog {
    id: loadingDialog
    title: qsTr( "SSO login" )
    description: qsTr( "Please follow the instructions in your web browser." )
    imageSource: __style.neutralMMSymbolImage

    additionalContent: MMBusyIndicator {
      running: true
      anchors.centerIn: parent
    }
  }

  Component.onCompleted: {
    __merginApi.requestSsoConfig()
  }
}
