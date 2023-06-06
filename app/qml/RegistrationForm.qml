

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick.Templates as T
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt5Compat.GraphicalEffects

import lc 1.0
import "." // import InputStyle singleton
import "./components"

/**
  * Body of the AuthPanel with the registration form - username, email, password, ...
  */
Rectangle {
  id: registerForm
  color: root.bgColor

  function clean() {
    registerName.text = ""
    email.text = ""
    passwordField.password.text = ""
    passwordConfirmField.password.text = ""
    acceptTOC.checked = false
  }

  Connections {
    target: __merginApi
    enabled: registerForm.visible

    // show error message under the respective field
    function onRegistrationFailed( msg, field ) {
      console.log( "onRegistrationFailed: " + msg )

      // clear previous error messages
      registerNameErrorText.text = ""
      emailErrorText.text = ""
      passwordFieldErrorText.text = ""
      passwordConfirmFieldErrorText.text = ""
      acceptTOCErrorText.text = ""
      errorText.text = ""

      if( field === RegistrationError.USERNAME ) {
        registerNameErrorText.text = msg
        registerName.focus = true
      }
      else if( field === RegistrationError.EMAIL ) {
        emailErrorText.text = msg
        email.focus = true
      }
      else if( field === RegistrationError.PASSWORD ) {
        passwordFieldErrorText.text = msg
        passwordField.password.focus = true
      }
      else if( field === RegistrationError.CONFIRM_PASSWORD ) {
        passwordConfirmFieldErrorText.text = msg
        passwordConfirmField.password.focus = true
      }
      else if( field === RegistrationError.TOC ) {
        acceptTOCErrorText.text = msg
        acceptTOC.focus = true
      }
      else if( field === RegistrationError.OTHER ) {
        errorText.text = msg
      }
    }
  }

  ScrollView {
    width: registerForm.width
    height: registerForm.height

  Column {
    id: columnLayout
    spacing: root.panelMargin / 2
    width: parent.width
    anchors.bottom: parent.bottom

    Image {
      source: InputStyle.mmLogoHorizontal
      height: fieldHeight * 0.8
      sourceSize.height: height
      anchors.horizontalCenter: parent.horizontalCenter
    }

    Row {
      id: row
      width: registerForm.width
      height: fieldHeight
      spacing: 0

      Rectangle {
        id: iconContainer
        height: fieldHeight
        width: fieldHeight
        color: root.bgColor

        Image {
          anchors.margins: root.panelMargin
          id: icon
          height: fieldHeight
          width: fieldHeight
          anchors.fill: parent
          source: InputStyle.accountIcon
          sourceSize.width: width
          sourceSize.height: height
          fillMode: Image.PreserveAspectFit
        }

        ColorOverlay {
          anchors.fill: icon
          source: icon
          color: registerNameErrorText.text.length>0 ? InputStyle.errorTextColor : root.fontColor
        }
      }

      TextField {
        id: registerName
        x: iconContainer.width
        width: parent.width - iconContainer.width
        height: fieldHeight
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: root.fontColor
        placeholderText: qsTr("Username")
        font.capitalization: Font.MixedCase
        inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
        background: Rectangle {
          color: root.bgColor
        }
      }
    }

    Rectangle {
      id: registerNameBorder
      color: registerNameErrorText.text.length>0 ? InputStyle.errorTextColor : InputStyle.panelBackgroundDark
      y: registerName.height - height
      height: 2 * __dp
      opacity: registerName.focus ? 1 : 0.6
      width: parent.width - fieldHeight / 2
      anchors.horizontalCenter: parent.horizontalCenter
    }

    ErrorText {
      id: registerNameErrorText
      width: parent.width - fieldHeight / 2
    }

    Row {
      width: registerForm.width
      height: fieldHeight
      spacing: 0

      Rectangle {
        id: iconContainer4
        height: fieldHeight
        width: fieldHeight
        color: root.bgColor

        Image {
          anchors.margins: (fieldHeight / 4)
          id: icon4
          height: fieldHeight
          width: fieldHeight
          anchors.fill: parent
          source: InputStyle.envelopeIcon
          sourceSize.width: width
          sourceSize.height: height
          fillMode: Image.PreserveAspectFit
        }

        ColorOverlay {
          anchors.fill: icon4
          source: icon4
          color: emailErrorText.text.length>0 ? InputStyle.errorTextColor : root.fontColor
        }
      }

      TextField {
        id: email
        width: parent.width - iconContainer.width
        height: fieldHeight
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: root.fontColor
        placeholderText: qsTr("Email")
        inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
        background: Rectangle {
          color: root.bgColor
        }
      }
    }

    Rectangle {
      id: emailBorder
      color: emailErrorText.text.length>0 ? InputStyle.errorTextColor : InputStyle.panelBackgroundDark
      height: 2 * __dp
      y: email.height - height
      opacity: email.focus ? 1 : 0.6
      width: registerForm.width - fieldHeight / 2
      anchors.horizontalCenter: parent.horizontalCenter
    }

    ErrorText {
      id: emailErrorText
      width: parent.width - fieldHeight / 2
    }

    PasswordField {
      id: passwordField
      width: registerForm.width
      height: fieldHeight
      fontColor: root.fontColor
      bgColor: root.bgColor
      isWrong: passwordFieldErrorText.text.length>0
    }

    Rectangle {
      id: passBorder
      color: passwordFieldErrorText.text.length>0 ? InputStyle.errorTextColor : InputStyle.panelBackgroundDark
      height: 2 * __dp
      y: fieldHeight - height
      opacity: passwordField.password.focus ? 1 : 0.6
      width: registerForm.width - fieldHeight / 2
      anchors.horizontalCenter: parent.horizontalCenter
    }

    ErrorText {
      id: passwordFieldErrorText
      width: parent.width - fieldHeight / 2
    }

    PasswordField {
      id: passwordConfirmField
      width: registerForm.width
      height: fieldHeight
      fontColor: root.fontColor
      bgColor: root.bgColor
      placeholderText:  qsTr('Confirm password')
      isWrong: passwordConfirmFieldErrorText.text.length>0
    }

    Rectangle {
      id: confirmPassBorder
      color: passwordConfirmFieldErrorText.text.length>0 ? InputStyle.errorTextColor : InputStyle.panelBackgroundDark
      height: 2 * __dp
      y: fieldHeight - height
      opacity: passwordConfirmField.password.focus ? 1 : 0.6
      width: registerForm.width - fieldHeight / 2
      anchors.horizontalCenter: parent.horizontalCenter
    }

    ErrorText {
      id: passwordConfirmFieldErrorText
      width: parent.width - fieldHeight / 2
    }

    Row {
      width: registerForm.width
      height: fieldHeight
      spacing: 0

      LeftCheckBox {
        id: acceptTOC
        anchors.margins: (fieldHeight / 4)
        baseColor: root.fontColor
        height: fieldHeight
        width: registerForm.width
        text: qsTr("I accept the Mergin %1Terms and Conditions%3 and %2Privacy Policy%3")
              .arg("<a href='"+ __inputHelp.merginTermsLink + "'>")
              .arg("<a href='"+ __inputHelp.privacyPolicyLink +"'>")
              .arg("</a>")
        shouldBeChecked: acceptTOCErrorText.text.length>0
      }
    }

    Rectangle {
      id: acceptTOCBorder
      color: acceptTOCErrorText.text.length>0 ? InputStyle.errorTextColor : InputStyle.panelBackgroundDark
      height: 2 * __dp
      y: acceptTOC.height - height
      opacity: acceptTOC.focus ? 1 : 0.6
      width: registerForm.width - fieldHeight / 2
      anchors.horizontalCenter: parent.horizontalCenter
    }

    ErrorText {
      id: acceptTOCErrorText
      width: parent.width - fieldHeight / 2
    }

    Button {
      id: registerButton
      enabled: !stackView.pending
      width: registerForm.width - 2 * root.panelMargin
      height: fieldHeight
      text: qsTr("Sign up")
      font.pixelSize: InputStyle.fontPixelSizeBig
      anchors.horizontalCenter: parent.horizontalCenter
      onClicked: {
          stackView.pending = true
          __merginApi.registerUser(registerName.text, email.text, passwordField.password.text,
                                   passwordConfirmField.password.text, acceptTOC.checked)
      }
      background: Rectangle {
        color: InputStyle.highlightColor
      }

      contentItem: Text {
        text: registerButton.text
        font: registerButton.font
        opacity: enabled ? 1.0 : 0.3
        color: root.bgColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
      }
    }

    }
  }
}
