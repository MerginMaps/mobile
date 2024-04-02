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
    string username,
    string email,
    string password,
    string passwordConfirm,
    bool tocAccept,
    bool newsletterSubscribe
  )

  pageHeader {
    title: qsTr( "Sign up" )

    titleFont: __style.h3
    color: __style.transparentColor
    topSpacing: Math.max( __style.safeAreaTop, __style.margin54 )
    baseHeaderHeight: __style.row80
    backButton.bgndColor: __style.lightGreenColor
  }

  background: MMAccountComponents.MMAuthPageBackground{}

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
        id: username

        width: parent.width

        title: qsTr( "Username" )
        bgColor: __style.lightGreenColor
        textFieldComponent.inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
      }

      MMTextInput {
        id: email

        width: parent.width

        title: qsTr( "Email" )
        bgColor: __style.lightGreenColor
        textFieldComponent.inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
      }

      MMPasswordInput {
        id: password

        width: parent.width

        title: qsTr( "Password" )
        bgColor: __style.lightGreenColor
      }

      MMPasswordInput {
        id: passwordConfirm

        width: parent.width

        title: qsTr( "Confirm password" )
        bgColor: __style.lightGreenColor
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

        disabled: root.pending

        onClicked: {
          root.signUpClicked(
                username.text,
                email.text,
                password.text,
                passwordConfirm.text,
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
    username.errorMsg = ""
    email.errorMsg = ""
    password.errorMsg = ""
    passwordConfirm.errorMsg = ""
    tocCheck.hasError = false

    if( field === MM.RegistrationError.USERNAME ) {
      username.errorMsg = msg
      username.focus = true
    }
    else if( field === MM.RegistrationError.EMAIL ) {
      email.errorMsg = msg
      email.focus = true
    }
    else if( field === MM.RegistrationError.PASSWORD ) {
      password.errorMsg = msg
      password.focus = true
    }
    else if( field === MM.RegistrationError.CONFIRM_PASSWORD ) {
      passwordConfirm.errorMsg = msg
      passwordConfirm.focus = true
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
