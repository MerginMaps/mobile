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
import "."  // import InputStyle singleton

Item {

  signal authFailed()

  property alias merginLink: merginLink
  property alias loginName: loginName
  property alias password: password
  /**
  * Suppose to be true if auth request is pending. Then busy indicator is running and
  * the loginButton is disabled.
  */
  property bool pending: false
  property string errorText: errorText

  property real fieldHeight: InputStyle.rowHeight
  property real panelMargin: fieldHeight/4
  property real toolbarHeight: InputStyle.rowHeightHeader
  property color fontColor: InputStyle.panelBackgroundDarker
  property color bgColor: "white"

  function close() {
    visible = false
    password.text = ""
    loginName.text = ""
  }

  Keys.onReleased: {

    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      console.log("Back button signal catched in AuthPanel!");

//      if (!activeProjectPath)

//      if (root.activeFocus)
      event.accepted = true;
      root.close()
      authFailed()
      password.focus = false;
      loginName.focus = false;
      root.focus = false;
    }
  }

  id: root
//  focus: true


  Pane {
    id: pane
    anchors.fill: parent
    background: Rectangle {
      color: root.bgColor
    }
    clip: true

    Rectangle {
      id: loginForm
      width: parent.width
      height: parent.height - staticPane.height
      color: root.bgColor
      anchors.bottom: staticPane.top
      anchors.bottomMargin: {
        Math.max(Qt.inputMethod.keyboardRectangle.height ?
                   Qt.inputMethod.keyboardRectangle.height - (staticPane.height + toolbarHeight + panelMargin):
                   0, 0)
      }

      Column {
        id: columnLayout
        spacing: root.panelMargin/2
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
          visible: !warningMsgContainer.visible
          color: root.fontColor
          y: loginName.height - height
          height: 2 * QgsQuick.Utils.dp
          opacity: loginName.focus ? 1 : 0.6
          width: parent.width - fieldHeight/2
          anchors.horizontalCenter: parent.horizontalCenter
        }

        Row {
          width: loginForm.width
          height: fieldHeight
          spacing: 0
          visible: !warningMsgContainer.visible

          Rectangle {
            id: iconContainer2
            height: fieldHeight
            width: fieldHeight
            color: root.bgColor

            Image {
              anchors.margins: (fieldHeight/4)
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
          width: loginForm.width - fieldHeight/2
          anchors.horizontalCenter: parent.horizontalCenter
        }

        Button {
          id: loginButton
          visible: !warningMsgContainer.visible
          enabled: !root.pending
          width: loginForm.width - 2* root.panelMargin
          height: fieldHeight
          text: qsTr("Sign in")
          font.pixelSize: loginButton.height/2
          anchors.horizontalCenter: parent.horizontalCenter
          onClicked: {
            root.pending = true
            __merginApi.authorize(loginName.text, password.text)
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


        // Mergin check
        Row {
          id: warningMsgContainer
          visible: __merginApi.apiVersionStatus !== MerginApiStatus.OK
          width: loginForm.width

          Text {
            id: pendingText
            width: parent.width
            text: {
              if (__merginApi.apiVersionStatus === MerginApiStatus.INCOMPATIBLE ) {
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
          height: fieldHeight/2

          Button {
            width: parent.height
            height: parent.height
            anchors.horizontalCenter: parent.horizontalCenter
            visible: __merginApi.apiVersionStatus === MerginApiStatus.INCOMPATIBLE ||
                     __merginApi.apiVersionStatus === MerginApiStatus.NOT_FOUND
            background: Rectangle {
              anchors.fill: parent
              color: InputStyle.fontColor
              radius: 2 * QgsQuick.Utils.dp
            }

            onClicked:__merginApi.pingMergin()

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

    // Static part
    Rectangle {
      id: staticPane
      width: parent.width
      height: childrenRect.height // parent.height - loginForm.height
      color: root.bgColor
      anchors.bottom: parent.bottom

      Column {
        //anchors.fill: parent
        spacing: root.panelMargin/2

        Button {
          id: signUpButton
          visible: !warningMsgContainer.visible
          width: loginForm.width - 2* root.panelMargin
          height: fieldHeight * 0.7
          text: qsTr("Sign up")
          font.pixelSize: signUpButton.height/2
          anchors.horizontalCenter: parent.horizontalCenter
          onClicked:Qt.openUrlExternally(__merginApi.apiRoot);
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

        Row {
          height: fieldHeight
          anchors.horizontalCenter: parent.horizontalCenter
          Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

          Item {
            id: iconContainerMergin
            height: fieldHeight/2
            width: fieldHeight/2

            MouseArea {
              width: iconContainerMergin.width
              height: iconContainerMergin.height
              onClicked: {
                merginLink.enabled = !merginLink.enabled
              }
            }

            Image {
              id: iconLink
              anchors.fill: parent
              anchors.margins: parent.height * 0.25
              source: merginLink.enabled ? 'check.svg' : InputStyle.editIcon
              sourceSize.width: width
              sourceSize.height: height
              fillMode: Image.PreserveAspectFit
            }

            ColorOverlay {
              anchors.fill: iconLink
              source: iconLink
              color: root.fontColor
            }
          }

          TextField {
            id: merginLink
            text: __merginApi.apiRoot
            height: fieldHeight/2
            color: root.fontColor
            verticalAlignment: Text.AlignVCenter
            enabled: false

            background: Rectangle {
              anchors.fill: parent
              color: enabled ? InputStyle.panelBackgroundLight : root.bgColor
            }

            onEnabledChanged: {
              if (!enabled && __merginApi.apiRoot !== merginLink.text) {
                __merginApi.apiRoot = merginLink.text
              }
            }
          }
        }
      }
    }
  }
}
