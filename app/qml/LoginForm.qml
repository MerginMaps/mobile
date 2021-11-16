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
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "." // import InputStyle singleton
import "components"

/**
  * Body of the AuthPanel with the login form - username and password
  * Also has a link to switch to register form and forgot password link
  */
Rectangle {
  id: loginForm
  property color bgColor
  property var fieldHeight
  property color fontColor
  property bool isKeyboardOpen: Qt.inputMethod.keyboardRectangle.height
  property real panelMargin

  function clean() {
    passwordField.password.text = "";
    loginName.text = "";
  }
  signal registrationRequested

  onIsKeyboardOpenChanged: if (!isKeyboardOpen) {
    if (loginName.focus)
      loginName.focus = false;
    if (passwordField.password.focus)
      passwordField.password.focus = false;
  }

  ScrollView {
    height: loginForm.height
    width: loginForm.width

    Column {
      id: columnLayout
      anchors.bottom: parent.bottom
      spacing: loginForm.panelMargin / 2
      width: loginForm.width

      Image {
        anchors.horizontalCenter: parent.horizontalCenter
        height: fieldHeight
        source: InputStyle.merginColorIcon
        sourceSize.height: height
      }
      TextHyperlink {
        id: merginInfo
        anchors.horizontalCenter: parent.horizontalCenter
        height: 2 * fieldHeight
        text: qsTr("%1Mergin%2 provides cloud-based sync between your mobile and desktop. Also use it to share your projects with others and work collaboratively.").arg("<a href='" + __inputHelp.merginWebLink + "'>").arg("</a>")
        visible: !loginName.activeFocus && !passwordField.password.activeFocus
        width: columnLayout.width
      }
      Row {
        id: row
        height: fieldHeight
        spacing: 0
        width: loginForm.width

        Rectangle {
          id: iconContainer
          color: loginForm.bgColor
          height: fieldHeight
          width: fieldHeight

          Image {
            id: icon
            anchors.fill: parent
            anchors.margins: loginForm.panelMargin
            fillMode: Image.PreserveAspectFit
            height: fieldHeight
            source: InputStyle.accountIcon
            sourceSize.height: height
            sourceSize.width: width
            width: fieldHeight
          }
          ColorOverlay {
            anchors.fill: icon
            color: loginForm.fontColor
            source: icon
          }
        }
        TextField {
          id: loginName
          color: loginForm.fontColor
          font.capitalization: Font.MixedCase
          font.pixelSize: InputStyle.fontPixelSizeNormal
          height: fieldHeight
          inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
          placeholderText: qsTr("Username")
          width: parent.width - iconContainer.width
          x: iconContainer.width

          onEditingFinished: focus = false

          background: Rectangle {
            color: loginForm.bgColor
          }
        }
      }
      Rectangle {
        id: loginNameBorder
        anchors.horizontalCenter: parent.horizontalCenter
        color: loginForm.fontColor
        height: 2 * QgsQuick.Utils.dp
        opacity: loginName.focus ? 1 : 0.6
        width: parent.width - fieldHeight / 2
        y: loginName.height - height
      }
      PasswordField {
        id: passwordField
        bgColor: loginForm.bgColor
        fontColor: loginForm.fontColor
        height: fieldHeight
        width: loginForm.width
      }
      Rectangle {
        id: passBorder
        anchors.horizontalCenter: parent.horizontalCenter
        color: loginForm.fontColor
        height: 2 * QgsQuick.Utils.dp
        opacity: passwordField.password.focus ? 1 : 0.6
        width: loginForm.width - fieldHeight / 2
        y: fieldHeight - height
      }
      Button {
        id: loginButton
        anchors.horizontalCenter: parent.horizontalCenter
        enabled: !stackView.pending
        font.pixelSize: InputStyle.fontPixelSizeTitle
        height: loginForm.fieldHeight
        text: qsTr("Sign in")
        width: loginForm.width - 2 * loginForm.panelMargin

        onClicked: {
          stackView.pending = true;
          __merginApi.authorize(loginName.text, passwordField.password.text);
        }

        background: Rectangle {
          color: InputStyle.highlightColor
        }
        contentItem: Text {
          color: loginForm.bgColor
          elide: Text.ElideRight
          font: loginButton.font
          horizontalAlignment: Text.AlignHCenter
          opacity: enabled ? 1.0 : 0.3
          text: loginButton.text
          verticalAlignment: Text.AlignVCenter
        }
      }
      Item {
        height: fieldHeight / 2
        width: parent.width

        Button {
          anchors.horizontalCenter: parent.horizontalCenter
          height: parent.height
          visible: __merginApi.apiVersionStatus === MerginApiStatus.INCOMPATIBLE || __merginApi.apiVersionStatus === MerginApiStatus.NOT_FOUND
          width: parent.height

          onClicked: __merginApi.pingMergin()

          Image {
            id: image
            anchors.centerIn: parent
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 0
            fillMode: Image.PreserveAspectFit
            height: width
            source: InputStyle.syncIcon
            sourceSize.height: height
            sourceSize.width: width
            visible: source
            width: parent.width * 0.75
          }
          ColorOverlay {
            anchors.fill: image
            color: "white"
            source: image
          }

          background: Rectangle {
            anchors.fill: parent
            color: InputStyle.fontColor
            radius: 2 * QgsQuick.Utils.dp
          }
        }
      }
      Button {
        id: signUpButton
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: InputStyle.fontPixelSizeSmall
        height: fieldHeight * 0.7
        text: qsTr("Sign up for free")
        width: loginForm.width - 2 * loginForm.panelMargin

        onClicked: loginForm.registrationRequested()

        background: Rectangle {
          color: loginForm.bgColor
        }
        contentItem: Text {
          color: InputStyle.highlightColor
          elide: Text.ElideRight
          font: signUpButton.font
          horizontalAlignment: Text.AlignHCenter
          text: signUpButton.text
          verticalAlignment: Text.AlignVCenter
        }
      }
      Button {
        id: resetPasswordButton
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: InputStyle.fontPixelSizeSmall
        height: fieldHeight * 0.7
        text: qsTr("Forgot password?")
        width: loginForm.width - 2 * loginForm.panelMargin

        onClicked: Qt.openUrlExternally(__merginApi.resetPasswordUrl())

        background: Rectangle {
          color: loginForm.bgColor
        }
        contentItem: Text {
          color: InputStyle.highlightColor
          elide: Text.ElideRight
          font: resetPasswordButton.font
          horizontalAlignment: Text.AlignHCenter
          text: resetPasswordButton.text
          verticalAlignment: Text.AlignVCenter
        }
      }
    }
  }
}
