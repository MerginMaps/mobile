import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import "../" // import InputStyle singleton

Item {
  id: delegateButtonContainer
  property var bgColor: InputStyle.highlightColor
  property real btnHeight: InputStyle.delegateBtnHeight
  property real btnWidth: delegateButtonContainer.height * 3
  property real cornerRadius: InputStyle.cornerRadius
  property var fontColor: "white"
  property var iconSource: ""
  property string text

  signal clicked

  SimpleTextWithIcon {
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    color: delegateButtonContainer.bgColor
    fontBold: true
    height: delegateButtonContainer.btnHeight
    icon.width: delegateButtonContainer.btnHeight / 2
    radius: delegateButtonContainer.cornerRadius
    source: delegateButtonContainer.iconSource
    text: delegateButtonContainer.text
    width: delegateButtonContainer.btnWidth

    onClicked: delegateButtonContainer.clicked()
  }
}
