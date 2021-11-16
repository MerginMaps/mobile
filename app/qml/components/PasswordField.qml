import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import lc 1.0
import "../" // import InputStyle singleton

Row {
  id: root
  spacing: 0

  property var fontColor
  property var bgColor
  property var password: password
  property string placeholderText: qsTr('Password')

  Rectangle {
    id: iconContainer2
    height: root.height
    width: root.height
    color: root.bgColor

    Image {
      anchors.margins: (root.height / 4)
      id: icon
      height: root.height
      width: root.height
      anchors.fill: parent
      source: InputStyle.lockIcon
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
    id: password
    width: loginForm.width - iconContainer.width - visibilityIconContainer.width
    height: fieldHeight
    font.pixelSize: InputStyle.fontPixelSizeNormal
    color: root.fontColor
    placeholderText: root.placeholderText
    echoMode: TextInput.Password
    inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
    font.capitalization: Font.MixedCase

    background: Rectangle {
      color: root.bgColor
    }

    onEditingFinished: focus = false
    onVisibleChanged: if (!password.visible) password.echoMode = TextInput.Password
  }

  Rectangle {
    id: visibilityIconContainer
    height: root.height
    width: root.height
    color: root.bgColor

    Image {
      anchors.margins: (fieldHeight / 4)
      id: visibilityIcon
      height: root.height
      width: root.height
      anchors.fill: parent
      source: password.echoMode === TextInput.Normal ? InputStyle.eyeSlashIcon : InputStyle.eyeIcon
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
      anchors.fill: visibilityIcon
      source: visibilityIcon
      color: root.fontColor
    }
  }
}
