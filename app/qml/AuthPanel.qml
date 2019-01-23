import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Item {
  property alias loginName: loginName
  property alias password: password
  property alias loginIndicator: loginIndicator
  property string errorText: errorText

  property real fieldHeight: InputStyle.fontPixelSizeTitle

  id: root

  Rectangle {
      width: parent.width
      height: parent.height
      color: "white"
      opacity: 0.8
  }

  Pane {
    id: pane

    width: parent.width * 0.9
    height: parent.height * 0.9
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter

    background: Rectangle {
      color: "#ffffff"
    }

    Item {
        id: loginForm
        width: parent.width
        height: parent.height

        Column {
          id: columnLayout
          width: parent.width
          height: parent.height
          anchors.top: parent.top
          anchors.topMargin: parent.height/3
          anchors.right: parent.right
          anchors.left: parent.left
          anchors.verticalCenter: parent.verticalCenter
          anchors.horizontalCenter: parent.horizontalCenter

          TextField {
            id: loginName
            width: parent.width
            height: fieldHeight
            //font.pixelSize: InputStyle.fontPixelSizeNormal
            placeholderText: qsTr("Username")
            enabled: !loginIndicator.running
            font.capitalization: Font.MixedCase

          }
          TextField {
            id: password
            width: parent.width
            height: fieldHeight
            //font.pixelSize: InputStyle.fontPixelSizeNormal
            placeholderText: qsTr("Password")
            echoMode: TextInput.Password
            enabled: !loginIndicator.running
            inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
            font.capitalization: Font.MixedCase
          }

          Button {
            id: loginButton
            width: parent.width
            height: fieldHeight //parent.height/3
            text: qsTr("Login")
            font.pixelSize: loginButton.height/2
            onClicked: {
              __merginApi.authorize(loginName.text, password.text)
              loginIndicator.running = true
            }
            enabled: !loginIndicator.running
          }

          Label {
            width: columnLayout.width/2
            height: columnLayout.height/2
            color: "red"
            text: errorText
          }
        }
      }
    }


  // TODO
  Image {
    id: passVisibility
    y: pane.y + loginName.height
    x: pane.x + pane.width
    height: password.height
    width: height
    source: "visibility.svg"
    sourceSize: Qt.size(width, height)
    fillMode: Image.PreserveAspectFit
    MouseArea {
      anchors.fill: parent
      onClicked: {
        if (password.echoMode === TextInput.Normal) {
          password.echoMode = TextInput.Password
          passVisibility.source = "visibility.svg"
        } else {
          password.echoMode = TextInput.Normal
          passVisibility.source = "visibility-off.svg"
        }
      }
    }
  }

  Rectangle {
    width: parent.width
    height: parent.height
    color: "black"
    opacity: 0.3
    visible: loginIndicator.running
    z: 100
  }

  BusyIndicator {
    id: loginIndicator
    width: root.height/4
    height: root.height/4
    running: false
    visible: running
    anchors.centerIn: parent
    z: 101
  }

}
