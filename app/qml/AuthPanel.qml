

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
import "."

// import InputStyle singleton
Item {

  signal authFailed
  signal back()

  property alias merginLink: merginLink
  // property alias loginName: loginName
  // property alias password: loginForm.password


  /**
  * Suppose to be true if auth request is pending. Then busy indicator is running and
  * the loginButton is disabled.
  */
  property bool pending: false
  property string errorText: errorText

  property real fieldHeight: InputStyle.rowHeight
  property real panelMargin: fieldHeight / 4
  property real toolbarHeight: InputStyle.rowHeightHeader
  property color fontColor: InputStyle.panelBackgroundDarker
  property color bgColor: "white"

  function close() {
    visible = false
    loginForm.clean()
    registrationForm.clean()
  }

  id: root
  states: [
    State {
      name: "login"
    },
    State {
      name: "register"
    }
  ]

  onStateChanged: {
    if (state === "login") {
      loginForm.visible = true
      loginForm.clean()
      registrationForm.visible = false
    } else // if (state === "register")
    {
      loginForm.visible = false
      registrationForm.visible = true
      registrationForm.clean()
    }
  }

  state: "login"

  PanelHeader {
    id: header
    height: InputStyle.rowHeightHeader
    width: root.width
    color: InputStyle.clrPanelMain
    rowHeight: InputStyle.rowHeightHeader
    titleText: root.state === "login" ? qsTr("Login") : qsTr("Register")

    onBack: root.back()
  }

  Pane {
    id: pane
    height: root.height - header.height
    width: root.width
    y: header.height
    background: Rectangle {
      color: root.bgColor
    }
    clip: true

    LoginForm {
      id: loginForm
      visible: !warningMsgContainer.visible
      height: Qt.inputMethod.visible ? parent.height + staticPane.height - Qt.inputMethod.keyboardRectangle.height : parent.height - staticPane.height
      width: parent.width

      onRegistrationRequested: {
        registrationForm.clean()
        root.state = "register"
      }
    }

    RegistrationForm {
      id: registrationForm
      visible: false
      height: Qt.inputMethod.visible ? parent.height + staticPane.height - Qt.inputMethod.keyboardRectangle.height : parent.height - staticPane.height
      width: parent.width
    }

    // Mergin check
    Row {
      id: warningMsgContainer
      visible: __merginApi.apiVersionStatus !== MerginApiStatus.OK
      width: parent.width

      Text {
        id: pendingText
        width: parent.width
        text: {
          if (__merginApi.apiVersionStatus === MerginApiStatus.INCOMPATIBLE) {
            qsTr("Please update Input to use the latest Mergin features.")
          } else if (__merginApi.apiVersionStatus === MerginApiStatus.PENDING) {
            ""
          } else {
            qsTr("Mergin is currently unavailable - please try again later.")
          }
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor
        wrapMode: Text.WordWrap
      }
    }

    // Mergin Server URI
    Rectangle {
      id: staticPane
      width: parent.width
      height: childrenRect.height // parent.height - loginForm.height
      color: root.bgColor
      anchors.bottom: parent.bottom

      Row {
        height: fieldHeight
        anchors.horizontalCenter: parent.horizontalCenter
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

        Item {
          id: iconContainerMergin
          height: fieldHeight / 2
          width: fieldHeight / 2

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
          height: fieldHeight / 2
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
