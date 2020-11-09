

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "." // import InputStyle singleton
import "./components"

/**
  * Body of the AuthPanel with the login form - username and password
  * Also has a link to switch to register form
  */
Rectangle {
  signal registrationRequested()

  id: loginForm
  color: root.bgColor

  function clean() {
    passwordField.password.text = ""
    loginName.text = ""
  }

  Column {
    id: columnLayout
    spacing: root.panelMargin / 2
    width: parent.width
    anchors.bottom: parent.bottom

    Image {
      source: "mergin_color.svg"
      height: fieldHeight
      sourceSize.height: height
      anchors.horizontalCenter: parent.horizontalCenter
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
        color: root.bgColor

        Image {
          anchors.margins: root.panelMargin
          id: icon
          height: fieldHeight
          width: fieldHeight
          anchors.fill: parent
          source: 'account.svg'
          sourceSize.width: width
          sourceSize.height: height
          fillMode: Image.PreserveAspectFit
        }

        ColorOverlay {
          anchors.fill: icon
          source: icon
          color: root.fontColor
        }
      }

      TextField {
        id: loginName
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
      id: loginNameBorder
      color: root.fontColor
      y: loginName.height - height
      height: 2 * QgsQuick.Utils.dp
      opacity: loginName.focus ? 1 : 0.6
      width: parent.width - fieldHeight / 2
      anchors.horizontalCenter: parent.horizontalCenter
    }


    PasswordField {
      id: passwordField
      width: loginForm.width
      height: fieldHeight
      fontColor: root.fontColor
      bgColor: root.bgColor
    }

    Rectangle {
      id: passBorder
      color: root.fontColor
      height: 2 * QgsQuick.Utils.dp
      y: fieldHeight - height
      opacity: passwordField.password.focus ? 1 : 0.6
      width: loginForm.width - fieldHeight / 2
      anchors.horizontalCenter: parent.horizontalCenter
    }

    Button {
      id: loginButton
      enabled: !stackView.pending
      width: loginForm.width - 2 * root.panelMargin
      height: fieldHeight
      text: qsTr("Sign in")
      font.pixelSize: InputStyle.fontPixelSizeTitle
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
        color: root.bgColor
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
          radius: 2 * QgsQuick.Utils.dp
        }

        onClicked: __merginApi.pingMergin()

        Image {
          id: image
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.centerIn: parent
          width: parent.width * 0.75
          height: width
          source: "sync.svg"
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

    Column {
      //anchors.fill: parent
      spacing: root.panelMargin / 2

      Button {
        id: signUpButton
        width: loginForm.width - 2 * root.panelMargin
        height: fieldHeight * 0.7
        text: qsTr("Sign up")
        font.pixelSize: InputStyle.fontPixelSizeSmall
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: loginForm.registrationRequested()
        background: Rectangle {
          color: root.bgColor
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
    }

    Column {
      spacing: root.panelMargin / 2

      Button {
        id: resetPasswordButton
        width: loginForm.width - 2 * root.panelMargin
        height: fieldHeight * 0.7
        text: qsTr("Reset password")
        font.pixelSize: InputStyle.fontPixelSizeSmall
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: Qt.openUrlExternally(__merginApi.resetPasswordURl());
        background: Rectangle {
          color: root.bgColor
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
