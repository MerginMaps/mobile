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
import "components"

// import InputStyle singleton
Item {
  id: root
  property color bgColor: "white"
  property string errorText: errorText
  property real fieldHeight: InputStyle.rowHeight
  property color fontColor: InputStyle.panelBackgroundDarker
  property alias merginLink: merginLink
  property real panelMargin: fieldHeight / 4

  /**
  * Suppose to be true if auth request is pending. Then busy indicator is running and
  * the loginButton is disabled.
  */
  property bool pending: false
  property real toolbarHeight: InputStyle.rowHeightHeader

  state: "login"

  signal authFailed
  signal back
  function close() {
    visible = false;
    loginForm.clean();
    registrationForm.clean();
  }

  onStateChanged: {
    if (state === "login") {
      loginForm.visible = true;
      loginForm.clean();
      registrationForm.visible = false;
    } else // if (state === "register")
    {
      loginForm.visible = false;
      registrationForm.visible = true;
      registrationForm.clean();
    }
  }

  PanelHeader {
    id: header
    color: InputStyle.clrPanelMain
    height: InputStyle.rowHeightHeader
    rowHeight: InputStyle.rowHeightHeader
    titleText: root.state === "login" ? qsTr("Login") : qsTr("Register")
    width: root.width

    onBack: root.back()
  }
  Pane {
    id: pane
    bottomPadding: 0
    clip: true
    height: root.height - header.height
    width: root.width
    y: header.height

    LoginForm {
      id: loginForm
      bgColor: root.bgColor
      fieldHeight: root.fieldHeight
      fontColor: root.fontColor
      height: Qt.inputMethod.visible ? parent.height + staticPane.height - Qt.inputMethod.keyboardRectangle.height : parent.height - staticPane.height
      panelMargin: root.panelMargin
      visible: !warningMsgContainer.visible
      width: parent.width

      onRegistrationRequested: {
        registrationForm.clean();
        root.state = "register";
      }
    }
    RegistrationForm {
      id: registrationForm
      height: Qt.inputMethod.visible ? parent.height + staticPane.height - Qt.inputMethod.keyboardRectangle.height : parent.height - staticPane.height
      visible: false
      width: parent.width
    }

    // Mergin check
    Row {
      id: warningMsgContainer
      visible: __merginApi.apiVersionStatus !== MerginApiStatus.OK
      width: parent.width

      Text {
        id: pendingText
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeNormal
        horizontalAlignment: Text.AlignHCenter
        text: {
          if (__merginApi.apiVersionStatus === MerginApiStatus.INCOMPATIBLE) {
            qsTr("Please update Input to use the latest Mergin features.");
          } else if (__merginApi.apiVersionStatus === MerginApiStatus.PENDING) {
            "";
          } else {
            qsTr("Mergin is currently unavailable - please try again later.");
          }
        }
        verticalAlignment: Text.AlignVCenter
        width: parent.width
        wrapMode: Text.WordWrap
      }
    }

    // Mergin Server URI
    Rectangle {
      id: staticPane
      anchors.bottom: parent.bottom
      color: root.bgColor
      height: childrenRect.height // parent.height - loginForm.height
      width: parent.width

      Row {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        height: fieldHeight

        Item {
          id: iconContainerMergin
          height: fieldHeight / 2
          width: fieldHeight / 2

          MouseArea {
            height: iconContainerMergin.height
            width: iconContainerMergin.width

            onClicked: {
              merginLink.enabled = !merginLink.enabled;
            }
          }
          Image {
            id: iconLink
            anchors.fill: parent
            anchors.margins: parent.height * 0.25
            fillMode: Image.PreserveAspectFit
            source: merginLink.enabled ? InputStyle.checkIcon : InputStyle.editIcon
            sourceSize.height: height
            sourceSize.width: width
          }
          ColorOverlay {
            anchors.fill: iconLink
            color: root.fontColor
            source: iconLink
          }
        }
        TextField {
          id: merginLink
          color: root.fontColor
          enabled: false
          height: fieldHeight / 2
          text: __merginApi.apiRoot
          verticalAlignment: Text.AlignVCenter

          onEnabledChanged: {
            if (!enabled && __merginApi.apiRoot !== merginLink.text) {
              __merginApi.apiRoot = merginLink.text;
            }
          }

          background: Rectangle {
            anchors.fill: parent
            color: enabled ? InputStyle.panelBackgroundLight : root.bgColor
          }
        }
      }
    }

    background: Rectangle {
      color: root.bgColor
    }
  }

  states: [
    State {
      name: "login"
    },
    State {
      name: "register"
    }
  ]
}
