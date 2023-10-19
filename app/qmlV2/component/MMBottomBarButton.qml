import QtQuick
import "../Style.js" as Style

Column {
  height: control.height

  Item {
    height: parent.height
    width: (buttonView.width - 2*Style.commonSpacing) / buttonView.model.count

    MouseArea {
      width: parent.width - Style.commonSpacing/2
      height: parent.height - Style.commonSpacing/2
      anchors.centerIn: parent
      clip: button !== MMBottomBar.Buttons.Save

      Image {
        source: {
          switch (button) {
          case MMBottomBar.Buttons.Delete: return Style.deleteIcon
          case MMBottomBar.Buttons.EditGeometry: return Style.editIcon
          case MMBottomBar.Buttons.Advanced: return Style.moreIcon
          case MMBottomBar.Buttons.Save: return Style.doneIcon
          }
        }
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 2 * Style.commonSpacing + (button === MMBottomBar.Buttons.Save ? 14 * __dp : 0)

        Rectangle {
          visible: button === MMBottomBar.Buttons.Save
          anchors.centerIn: parent
          width: 56 * __dp
          height: width
          radius: width / 2
          color: Style.transparent
          border.color: Style.grass
          border.width: 14 * __dp
        }
      }
      Text {
        text: {
          switch (button) {
          case MMBottomBar.Buttons.Delete: return qsTr("Delete")
          case MMBottomBar.Buttons.EditGeometry: return qsTr("Edit Geometry")
          case MMBottomBar.Buttons.Advanced: return qsTr("Advanced")
          case MMBottomBar.Buttons.Save: return qsTr("Save")
          }
        }
        width: parent.width
        color: Style.white
        font: Qt.font(Style.t4)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Style.commonSpacing
        horizontalAlignment: Text.AlignHCenter
      }
    }
  }
}
