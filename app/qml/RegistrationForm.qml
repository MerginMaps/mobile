

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
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "."

// import InputStyle singleton
Rectangle {
  id: registerForm
  width: parent.width
  height: parent.height - staticPane.height
  color: root.bgColor
  anchors.bottom: staticPane.top
  anchors.bottomMargin: {
    Math.max(
          Qt.inputMethod.keyboardRectangle.height ? Qt.inputMethod.keyboardRectangle.height
                                                    - (staticPane.height + toolbarHeight
                                                       + panelMargin) : 0, 0)
  }

  function clean() {
    registerName.text = ""
    email.text = ""
    password.text = ""
    passwordConfirm.text = ""
    acceptTOC.checked = false
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
      width: registerForm.width
      height: fieldHeight
      visible: !warningMsgContainer.visible
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
      visible: !warningMsgContainer.visible
      color: root.fontColor
      y: registerName.height - height
      height: 2 * QgsQuick.Utils.dp
      opacity: registerName.focus ? 1 : 0.6
      width: parent.width - fieldHeight / 2
      anchors.horizontalCenter: parent.horizontalCenter
    }

    Row {
      width: registerForm.width
      height: fieldHeight
      spacing: 0
      visible: !warningMsgContainer.visible

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
          source: 'envelope-solid.svg'
          sourceSize.width: width
          sourceSize.height: height
          fillMode: Image.PreserveAspectFit
        }

        ColorOverlay {
          anchors.fill: icon4
          source: icon4
          color: root.fontColor
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
      visible: !warningMsgContainer.visible
      color: InputStyle.panelBackgroundDark
      height: 2 * QgsQuick.Utils.dp
      y: email.height - height
      opacity: email.focus ? 1 : 0.6
      width: registerForm.width - fieldHeight / 2
      anchors.horizontalCenter: parent.horizontalCenter
    }

    Row {
      width: registerForm.width
      height: fieldHeight
      spacing: 0
      visible: !warningMsgContainer.visible

      Rectangle {
        id: iconContainer2
        height: fieldHeight
        width: fieldHeight
        color: root.bgColor

        Image {
          anchors.margins: (fieldHeight / 4)
          id: icon2
          height: fieldHeight
          width: fieldHeight
          anchors.fill: parent
          source: 'lock.svg'
          sourceSize.width: width
          sourceSize.height: height
          fillMode: Image.PreserveAspectFit

          MouseArea {
            anchors.fill: parent
            onClicked: {
              if (password.echoMode === TextInput.Normal) {
                password.echoMode = TextInput.Password
              } else {
                password.echoMode = TextInput.Normal
              }
            }
          }
        }

        ColorOverlay {
          anchors.fill: icon2
          source: icon2
          color: root.fontColor
        }
      }

      TextField {
        id: password
        width: parent.width - iconContainer.width
        height: fieldHeight
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: root.fontColor
        placeholderText: qsTr("Password")
        echoMode: TextInput.Password
        inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
        font.capitalization: Font.MixedCase

        background: Rectangle {
          color: root.bgColor
        }
      }
    }

    Rectangle {
      id: passBorder
      visible: !warningMsgContainer.visible
      color: InputStyle.panelBackgroundDark
      height: 2 * QgsQuick.Utils.dp
      y: password.height - height
      opacity: password.focus ? 1 : 0.6
      width: registerForm.width - fieldHeight / 2
      anchors.horizontalCenter: parent.horizontalCenter
    }

    Row {
      width: registerForm.width
      height: fieldHeight
      spacing: 0
      visible: !warningMsgContainer.visible

      Rectangle {
        id: iconContainer3
        height: fieldHeight
        width: fieldHeight
        color: root.bgColor

        Image {
          anchors.margins: (fieldHeight / 4)
          id: icon3
          height: fieldHeight
          width: fieldHeight
          anchors.fill: parent
          source: 'lock.svg'
          sourceSize.width: width
          sourceSize.height: height
          fillMode: Image.PreserveAspectFit

          MouseArea {
            anchors.fill: parent
            onClicked: {
              if (passwordConfirm.echoMode === TextInput.Normal) {
                passwordConfirm.echoMode = TextInput.Password
              } else {
                passwordConfirm.echoMode = TextInput.Normal
              }
            }
          }
        }

        ColorOverlay {
          anchors.fill: icon3
          source: icon3
          color: root.fontColor
        }
      }

      TextField {
        id: passwordConfirm
        width: parent.width - iconContainer.width
        height: fieldHeight
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: root.fontColor
        placeholderText: qsTr("Confirm Password")
        echoMode: TextInput.Password
        inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
        font.capitalization: Font.MixedCase

        background: Rectangle {
          color: root.bgColor
        }
      }
    }

    Rectangle {
      id: confirmPassBorder
      visible: !warningMsgContainer.visible
      color: InputStyle.panelBackgroundDark
      height: 2 * QgsQuick.Utils.dp
      y: passwordConfirm.height - height
      opacity: passwordConfirm.focus ? 1 : 0.6
      width: registerForm.width - fieldHeight / 2
      anchors.horizontalCenter: parent.horizontalCenter
    }

    Row {
      width: registerForm.width
      height: fieldHeight
      spacing: 0
      visible: !warningMsgContainer.visible

      LeftCheckBox {
        id: acceptTOC
        anchors.margins: (fieldHeight / 4)
        baseColor: root.fontColor
        height: fieldHeight
        width: registerForm.width
        text: "<style>a:link { color: " + root.fontColor
              + "; text-decoration: underline; }</style>" + qsTr(
                "Accept <a href='%1'>Terms</a> and <a href='%2'>Privacy Policy</a>").arg(
                __merginApi.apiRoot + "assets/EULA.pdf").arg(
                "https://github.com/lutraconsulting/input/blob/master/privacy_policy.md")
      }
    }

    Rectangle {
      id: acceptTOCBorder
      visible: !warningMsgContainer.visible
      color: InputStyle.panelBackgroundDark
      height: 2 * QgsQuick.Utils.dp
      y: acceptTOC.height - height
      opacity: acceptTOC.focus ? 1 : 0.6
      width: registerForm.width - fieldHeight / 2
      anchors.horizontalCenter: parent.horizontalCenter
    }

    Button {
      id: registerButton
      visible: !warningMsgContainer.visible
      enabled: !root.pending
      width: registerForm.width - 2 * root.panelMargin
      height: fieldHeight
      text: qsTr("Sign up")
      font.pixelSize: registerButton.height / 2
      anchors.horizontalCenter: parent.horizontalCenter
      onClicked: {
        root.pending = true
        __merginApi.registerUser(registerName.text, email.text, password.text,
                                 passwordConfirm.text, acceptTOC.checked)
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

    // Mergin check
    Row {
      id: warningMsgContainer
      visible: __merginApi.apiVersionStatus !== MerginApiStatus.OK
      width: registerForm.width

      Text {
        id: pendingText
        width: parent.width
        text: {
          if (__merginApi.apiVersionStatus === MerginApiStatus.INCOMPATIBLE) {
            qsTr("Mergin server has been updated. Please, update Input app to enable Mergin functionality.")
          } else if (__merginApi.apiVersionStatus === MerginApiStatus.PENDING) {
            ""
          } else {
            qsTr("Mergin server unavailable.")
          }
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor
        wrapMode: Text.WordWrap
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
  }
}
