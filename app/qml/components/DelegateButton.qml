import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import "./.." // import InputStyle singleton
Item {
  signal clicked

  property string text
  property real cornerRadius: InputStyle.cornerRadius
  property var bgColor: InputStyle.highlightColor
  property var fontColor: "white"
  property real btnWidth: delegateButtonContainer.height * 3
  property real btnHeight: InputStyle.delegateBtnHeight
  property var iconSource: ""

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

    contentItem: Row {
      anchors.fill: parent
      spacing: 0

      Symbol {
        id: icon
        visible: !!delegateButtonContainer.iconSource
        height: visible ? delegateButtonContainer.btnHeight : 0
        width: height
        iconSize: height/2
        source: delegateButtonContainer.iconSource
        fontColor: delegateButtonContainer.fontColor
      }

      Text {
        height: delegateButtonContainer.btnHeight
        width: delegateButtonContainer.btnWidth - icon.width
        text: delegateButton.text
        font: delegateButton.font
        color: delegateButtonContainer.fontColor
        horizontalAlignment: icon.visible ? Text.AlignLeft : Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
      }
    }
  }
}
