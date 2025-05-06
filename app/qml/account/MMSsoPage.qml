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

  property bool focusOnBrowser: false

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
      }

      MMButton {
        width: parent.width

        text: qsTr( "Sign in" )

        enabled: email.text.match( "\\S+@\\S+\\.\\S+" )

        onClicked: {
          root.signInClicked( email.text )
        }
      }

      MMText {
        visible: root.focusOnBrowser

        text: qsTr( "Please follow the instructions on your web browser" )
      }

      MMHlineText {
        width: parent.width

        title: qsTr("Login with password?")
      }

      MMButton {
        width: parent.width

        text: qsTr( "Login with password" )

        type: MMButton.Types.Secondary

        onClicked: root.loginWithPasswordClicked()
      }

      MMListFooterSpacer {}
    }
  }

  Component.onCompleted: {
    console.log( "SSO page completed!" )
    __merginApi.requestSsoLogin()
  }
}
