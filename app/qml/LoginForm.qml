

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7
import QtQuick.Controls 2.9
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0

import lc 1.0
import "." // import InputStyle singleton
import "./components"

/**
  * Body of the AuthPanel with the login form - username and password
  * Also has a link to switch to register form and forgot password link
  */
Rectangle {
  signal registrationRequested()

  id: loginForm
  property real panelMargin
  property color bgColor
  property color fontColor
  property var fieldHeight

  property bool isKeyboardOpen: Qt.inputMethod.keyboardRectangle.height
  onIsKeyboardOpenChanged: if (!isKeyboardOpen) {
      if (loginName.focus)
        loginName.focus = false
      if (passwordField.password.focus)
        passwordField.password.focus = false
    }

  function clean() {
    passwordField.password.text = ""
    loginName.text = ""
  }

  ScrollView {
    width: loginForm.width
    height: loginForm.height

    Column {
      id: columnLayout
      spacing: loginForm.panelMargin / 2
      width: loginForm.width
      anchors.bottom: parent.bottom

      Image {
        source: InputStyle.merginColorIcon
        height: fieldHeight
        sourceSize.height: height
        anchors.horizontalCenter: parent.horizontalCenter
      }

      TextHyperlink {
        id: merginInfo
        width: columnLayout.width
        visible: !loginName.activeFocus && !passwordField.password.activeFocus
        height: 2 * fieldHeight
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("%1Mergin%2 provides cloud-based sync between your mobile and desktop. Also use it to share your projects with others and work collaboratively.")
                  .arg("<a href='" + __inputHelp.merginWebLink + "'>")
                  .arg("</a>")
      }

      Row {
        id: row
        width: loginForm.width
        height: fieldHeight
        spacing: 0

        Rectangle {
          id: iconContainer
          height: fieldHeight
          width: fieldHeight
          color: loginForm.bgColor

          Image {
            anchors.margins: loginForm.panelMargin
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
            color: loginForm.fontColor
          }
        }

        TextField {
          id: loginName
          x: iconContainer.width
          width: parent.width - iconContainer.width
          height: fieldHeight
          font.pixelSize: InputStyle.fontPixelSizeNormal
          color: loginForm.fontColor
          placeholderText: qsTr("Username")
          font.capitalization: Font.MixedCase
          inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
          background: Rectangle {
            color: loginForm.bgColor
          }
          onEditingFinished: focus = false
        }
      }

      Rectangle {
        id: loginNameBorder
        color: loginForm.fontColor
        y: loginName.height - height
        height: 2 * __dp
        opacity: loginName.focus ? 1 : 0.6
        width: parent.width - fieldHeight / 2
        anchors.horizontalCenter: parent.horizontalCenter
      }


      PasswordField {
        id: passwordField
        width: loginForm.width
        height: fieldHeight
        fontColor: loginForm.fontColor
        bgColor: loginForm.bgColor
      }

      Rectangle {
        id: passBorder
        color: loginForm.fontColor
        height: 2 * __dp
        y: fieldHeight - height
        opacity: passwordField.password.focus ? 1 : 0.6
        width: loginForm.width - fieldHeight / 2
        anchors.horizontalCenter: parent.horizontalCenter
      }

      Button {
        id: loginButton
        enabled: !stackView.pending
        width: loginForm.width - 2 * loginForm.panelMargin
        height: loginForm.fieldHeight
        text: qsTr("Sign in")
        font.pixelSize: InputStyle.fontPixelSizeBig
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
          stackView.pending = true
          __merginApi.authorize(loginName.text, passwordField.password.text)
        }
        background: Rectangle {
          color: InputStyle.highlightColor
        }

        contentItem: Text {
          text: loginButton.text
          font: loginButton.font
          opacity: enabled ? 1.0 : 0.3
          color: loginForm.bgColor
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideRight
        }
      }

      Item {
        width: parent.width
        height: fieldHeight / 2

        Button {
          width: parent.height
          height: parent.height
          anchors.horizontalCenter: parent.horizontalCenter
          visible: __merginApi.apiVersionStatus === MerginApiStatus.INCOMPATIBLE
                   || __merginApi.apiVersionStatus === MerginApiStatus.NOT_FOUND
          background: Rectangle {
            anchors.fill: parent
            color: InputStyle.fontColor
            radius: 2 * __dp
          }

          onClicked: __merginApi.pingMergin()

          Image {
            id: image
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.centerIn: parent
            width: parent.width * 0.75
            height: width
            source: InputStyle.syncIcon
            sourceSize.width: width
            sourceSize.height: height
            visible: source
            anchors.topMargin: 0
            fillMode: Image.PreserveAspectFit
          }

          ColorOverlay {
            anchors.fill: image
            source: image
            color: "white"
          }
        }
      }

      Button {
        id: signUpButton
        width: loginForm.width - 2 * loginForm.panelMargin
        height: fieldHeight * 0.7
        text: qsTr("Sign up for free")
        font.pixelSize: InputStyle.fontPixelSizeSmall
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: loginForm.registrationRequested()
        background: Rectangle {
          color: loginForm.bgColor
        }

        contentItem: Text {
          text: signUpButton.text
          font: signUpButton.font
          color: InputStyle.highlightColor
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideRight
        }
      }

      Button {
        id: resetPasswordButton
        width: loginForm.width - 2 * loginForm.panelMargin
        height: fieldHeight * 0.7
        text: qsTr("Forgot password?")
        font.pixelSize: InputStyle.fontPixelSizeSmall
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: Qt.openUrlExternally(__merginApi.resetPasswordUrl());
        background: Rectangle {
          color: loginForm.bgColor
        }

        contentItem: Text {
          text: resetPasswordButton.text
          font: resetPasswordButton.font
          color: InputStyle.highlightColor
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideRight
        }
      }
    }
  }
}
