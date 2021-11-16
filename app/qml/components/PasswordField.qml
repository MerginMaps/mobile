import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import lc 1.0
import "../" // import InputStyle singleton

Row {
  id: root
  property var bgColor
  property var fontColor
  property var password: password
  property string placeholderText: qsTr('Password')

  spacing: 0

  Rectangle {
    id: iconContainer2
    color: root.bgColor
    height: root.height
    width: root.height

    Image {
      id: icon
      anchors.fill: parent
      anchors.margins: (root.height / 4)
      fillMode: Image.PreserveAspectFit
      height: root.height
      source: InputStyle.lockIcon
      sourceSize.height: height
      sourceSize.width: width
      width: root.height
    }
    ColorOverlay {
      anchors.fill: icon
      color: root.fontColor
      source: icon
    }
  }
  TextField {
    id: password
    color: root.fontColor
    echoMode: TextInput.Password
    font.capitalization: Font.MixedCase
    font.pixelSize: InputStyle.fontPixelSizeNormal
    height: fieldHeight
    inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
    placeholderText: root.placeholderText
    width: loginForm.width - iconContainer.width - visibilityIconContainer.width

    onEditingFinished: focus = false
    onVisibleChanged: if (!password.visible)
      password.echoMode = TextInput.Password

    background: Rectangle {
      color: root.bgColor
    }
  }
  Rectangle {
    id: visibilityIconContainer
    color: root.bgColor
    height: root.height
    width: root.height

    Image {
      id: visibilityIcon
      anchors.fill: parent
      anchors.margins: (fieldHeight / 4)
      fillMode: Image.PreserveAspectFit
      height: root.height
      source: password.echoMode === TextInput.Normal ? InputStyle.eyeSlashIcon : InputStyle.eyeIcon
      sourceSize.height: height
      sourceSize.width: width
      width: root.height

      MouseArea {
        anchors.fill: parent

        onClicked: {
          if (password.echoMode === TextInput.Normal) {
            password.echoMode = TextInput.Password;
          } else {
            password.echoMode = TextInput.Normal;
          }
        }
      }
    }
    ColorOverlay {
      anchors.fill: visibilityIcon
      color: root.fontColor
      source: visibilityIcon
    }
  }
}
