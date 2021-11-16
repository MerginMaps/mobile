import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import ".." // import InputStyle singleton
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

  SimpleTextWithIcon {
    onClicked: delegateButtonContainer.clicked()
    height: delegateButtonContainer.btnHeight
    width: delegateButtonContainer.btnWidth
    source: delegateButtonContainer.iconSource
    text: delegateButtonContainer.text
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    icon.width: delegateButtonContainer.btnHeight/2
    radius: delegateButtonContainer.cornerRadius
    color: delegateButtonContainer.bgColor
    fontBold: true
  }

}
