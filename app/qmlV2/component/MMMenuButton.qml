import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import "../Style.js" as Style
import "."

Item {
  height: Style.menuDrawerHeight

  Rectangle {
    anchors.top: parent.top
    width: parent.width
    height: 1 * __dp
    color: Style.gray
    visible: index
  }

  Row {
    height: parent.height
    spacing: Style.commonSpacing
    MMIcon {
      height: parent.height
      color: Style.forest
      source: {
        switch (button) {
        case MMBottomBar.Buttons.Delete: return Style.deleteIcon
        case MMBottomBar.Buttons.EditGeometry: return Style.editIcon
        case MMBottomBar.Buttons.Advanced: return Style.moreIcon
        case MMBottomBar.Buttons.Save: return Style.doneIcon
        }
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
      color: Style.forest
      font: Qt.font(Style.t3)
      verticalAlignment: Text.AlignVCenter
      height: parent.height
    }
  }
}
