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

//
// TODO: Change API root is missing here! - Do we need it?
//

MMPage {
  id: root

  /**
  * Suppose to be true if register request is pending. Then busy indicator is running and
  * the sign up button is disabled.
  */
  property bool pending: false

  property string tocString

  signal signInClicked
  signal signUpClicked(
    string email,
    string password,
    bool tocAccept,
    bool newsletterSubscribe
  )

  pageHeader {
    title: qsTr( "Sign up" )
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

        title: qsTr( "Email" )
        textField.inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhEmailCharactersOnly
      }

      MMPasswordInput {
        id: password

        width: parent.width

        title: qsTr( "Password" )
      }

      MMCheckBox {
        id: tocCheck

        width: parent.width

        text: root.tocString
      }

      MMCheckBox {
        id: newsletterCheck

        width: parent.width
        checked: true

        text: qsTr( "I want to subscribe to the newsletter" )
      }

      MMButton {
        width: parent.width

        text: qsTr( "Sign up" )

        enabled: !root.pending

        onClicked: {
          root.signUpClicked(
                email.text,
                password.text,
                tocCheck.checked,
                newsletterCheck.checked
                )
        }
      }

      MMHlineText {
        width: parent.width

        title: qsTr("Already have an account?")
      }

      MMButton {
        width: parent.width

        text: qsTr( "Sign in" )

        type: MMButton.Types.Secondary

        onClicked: root.signInClicked()
      }

      MMListFooterSpacer {}
    }
  }

  // show error message under the respective field
  function showErrorMessage( msg, field ) {

    // clear previous error messages
    email.errorMsg = ""
    password.errorMsg = ""
    tocCheck.hasError = false

    if( field === MM.RegistrationError.EMAIL ) {
      email.errorMsg = msg
      email.focus = true
    }
    else if( field === MM.RegistrationError.PASSWORD ) {
      password.errorMsg = msg
      password.focus = true
    }
    else if( field === MM.RegistrationError.TOC ) {
       tocCheck.hasError = true
      __notificationModel.addError( msg )
    }
    else if( field === MM.RegistrationError.OTHER ) {
      __notificationModel.addError( msg )
    }
  }
}
