import QtQuick 2.7
import QtQuick.Controls 2.2
import "./.."  // import InputStyle singleton

Item {
  signal clicked()

  property string text
  property real cornerRadius: InputStyle.cornerRadius
  property var bgColor: InputStyle.highlightColor
  property var textColor: "white"
  property real btnWidth: delegateButtonContainer.height * 2
  property real btnHeight: delegateButtonContainer.height * 0.8

  id: delegateButtonContainer

  Button {
    id: delegateButton
    text: delegateButtonContainer.text
    height: delegateButtonContainer.btnHeight
    width: delegateButtonContainer.btnWidth
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    font.pixelSize: InputStyle.fontPixelSizeTitle

    background: Rectangle {
      color: delegateButtonContainer.bgColor
      radius: delegateButtonContainer.cornerRadius
    }

    onClicked: delegateButtonContainer.clicked()

    contentItem: Text {
      text: delegateButton.text
      font: delegateButton.font
      color: delegateButtonContainer.textColor
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      elide: Text.ElideRight
    }
  }
}
