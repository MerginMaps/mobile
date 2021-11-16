/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick.Templates 2.1 as T
import QtQuick 2.7
import QtQuick.Controls 2.7
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "." // import InputStyle singleton
import "components"

/**
  * Body of the AuthPanel with the registration form - username, email, password, ...
  */
Rectangle {
  id: registerForm
  color: root.bgColor

  function clean() {
    registerName.text = "";
    email.text = "";
    passwordField.password.text = "";
    passwordConfirmField.password.text = "";
    acceptTOC.checked = false;
  }

  ScrollView {
    height: registerForm.height
    width: registerForm.width

    Column {
      id: columnLayout
      anchors.bottom: parent.bottom
      spacing: root.panelMargin / 2
      width: parent.width

      Image {
        anchors.horizontalCenter: parent.horizontalCenter
        height: fieldHeight
        source: InputStyle.merginColorIcon
        sourceSize.height: height
      }
      Row {
        id: row
        height: fieldHeight
        spacing: 0
        width: registerForm.width

        Rectangle {
          id: iconContainer
          color: root.bgColor
          height: fieldHeight
          width: fieldHeight

          Image {
            id: icon
            anchors.fill: parent
            anchors.margins: root.panelMargin
            fillMode: Image.PreserveAspectFit
            height: fieldHeight
            source: InputStyle.accountIcon
            sourceSize.height: height
            sourceSize.width: width
            width: fieldHeight
          }
          ColorOverlay {
            anchors.fill: icon
            color: root.fontColor
            source: icon
          }
        }
        TextField {
          id: registerName
          color: root.fontColor
          font.capitalization: Font.MixedCase
          font.pixelSize: InputStyle.fontPixelSizeNormal
          height: fieldHeight
          inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
          placeholderText: qsTr("Username")
          width: parent.width - iconContainer.width
          x: iconContainer.width

          background: Rectangle {
            color: root.bgColor
          }
        }
      }
      Rectangle {
        id: registerNameBorder
        anchors.horizontalCenter: parent.horizontalCenter
        color: root.fontColor
        height: 2 * QgsQuick.Utils.dp
        opacity: registerName.focus ? 1 : 0.6
        width: parent.width - fieldHeight / 2
        y: registerName.height - height
      }
      Row {
        height: fieldHeight
        spacing: 0
        width: registerForm.width

        Rectangle {
          id: iconContainer4
          color: root.bgColor
          height: fieldHeight
          width: fieldHeight

          Image {
            id: icon4
            anchors.fill: parent
            anchors.margins: (fieldHeight / 4)
            fillMode: Image.PreserveAspectFit
            height: fieldHeight
            source: InputStyle.envelopeIcon
            sourceSize.height: height
            sourceSize.width: width
            width: fieldHeight
          }
          ColorOverlay {
            anchors.fill: icon4
            color: root.fontColor
            source: icon4
          }
        }
        TextField {
          id: email
          color: root.fontColor
          font.pixelSize: InputStyle.fontPixelSizeNormal
          height: fieldHeight
          inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
          placeholderText: qsTr("Email")
          width: parent.width - iconContainer.width

          background: Rectangle {
            color: root.bgColor
          }
        }
      }
      Rectangle {
        id: emailBorder
        anchors.horizontalCenter: parent.horizontalCenter
        color: InputStyle.panelBackgroundDark
        height: 2 * QgsQuick.Utils.dp
        opacity: email.focus ? 1 : 0.6
        width: registerForm.width - fieldHeight / 2
        y: email.height - height
      }
      PasswordField {
        id: passwordField
        bgColor: root.bgColor
        fontColor: root.fontColor
        height: fieldHeight
        width: registerForm.width
      }
      Rectangle {
        id: passBorder
        anchors.horizontalCenter: parent.horizontalCenter
        color: InputStyle.panelBackgroundDark
        height: 2 * QgsQuick.Utils.dp
        opacity: passwordField.password.focus ? 1 : 0.6
        width: registerForm.width - fieldHeight / 2
        y: fieldHeight - height
      }
      PasswordField {
        id: passwordConfirmField
        bgColor: root.bgColor
        fontColor: root.fontColor
        height: fieldHeight
        placeholderText: qsTr('Confirm password')
        width: registerForm.width
      }
      Rectangle {
        id: confirmPassBorder
        anchors.horizontalCenter: parent.horizontalCenter
        color: InputStyle.panelBackgroundDark
        height: 2 * QgsQuick.Utils.dp
        opacity: passwordConfirmField.password.focus ? 1 : 0.6
        width: registerForm.width - fieldHeight / 2
        y: fieldHeight - height
      }
      Row {
        height: fieldHeight
        spacing: 0
        width: registerForm.width

        LeftCheckBox {
          id: acceptTOC
          anchors.margins: (fieldHeight / 4)
          baseColor: root.fontColor
          height: fieldHeight
          text: qsTr("I accept the Mergin %1Terms and Conditions%3 and %2Privacy Policy%3").arg("<a href='" + __inputHelp.merginTermsLink + "'>").arg("<a href='" + __inputHelp.privacyPolicyLink + "'>").arg("</a>")
          width: registerForm.width
        }
      }
      Rectangle {
        id: acceptTOCBorder
        anchors.horizontalCenter: parent.horizontalCenter
        color: InputStyle.panelBackgroundDark
        height: 2 * QgsQuick.Utils.dp
        opacity: acceptTOC.focus ? 1 : 0.6
        width: registerForm.width - fieldHeight / 2
        y: acceptTOC.height - height
      }
      Button {
        id: registerButton
        anchors.horizontalCenter: parent.horizontalCenter
        enabled: !stackView.pending
        font.pixelSize: InputStyle.fontPixelSizeTitle
        height: fieldHeight
        text: qsTr("Sign up")
        width: registerForm.width - 2 * root.panelMargin

        onClicked: {
          stackView.pending = true;
          __merginApi.registerUser(registerName.text, email.text, passwordField.password.text, passwordConfirmField.password.text, acceptTOC.checked);
        }

        background: Rectangle {
          color: InputStyle.highlightColor
        }
        contentItem: Text {
          color: root.bgColor
          elide: Text.ElideRight
          font: registerButton.font
          horizontalAlignment: Text.AlignHCenter
          opacity: enabled ? 1.0 : 0.3
          text: registerButton.text
          verticalAlignment: Text.AlignVCenter
        }
      }
    }
  }
}
