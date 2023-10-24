import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import "../Style.js" as Style

Item {
  id: control

  signal clicked(var button)

  height: parent.height
  width: (buttonView.width - 2*Style.commonSpacing) / buttonView.model.count

  Button {
    id: bigButton

    anchors.centerIn: parent
    width: parent.width
    visible: button === MMBottomBar.Buttons.BigEdit

    contentItem: Row {
//      Image {
//        source: {
//          switch (button) {
//          case MMBottomBar.Buttons.BigEdit: return Style.editIcon
//          }
//        }
//        anchors.horizontalCenter: parent.horizontalCenter
//        anchors.bottom: parent.bottom
//        anchors.bottomMargin: 2 * Style.commonSpacing + (button === MMBottomBar.Buttons.Save ? 14 * __dp : 0)
//      }
      Text {
        font: Qt.font(Style.t3)
        text: qsTr("Edit")
        leftPadding: 32 * __dp
        rightPadding: 32 * __dp
        topPadding: 10 * __dp
        bottomPadding: 10 * __dp
        color: Style.forest
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
      }
    }

    background: Rectangle {
      color: Style.grass
      radius: height / 2
    }
  }

  Rectangle {
    id: smallButton

    width: parent.width - Style.commonSpacing/2
    height: parent.height - Style.commonSpacing/2
    anchors.centerIn: parent
    clip: button !== MMBottomBar.Buttons.Save
    color: Style.transparent

    visible: button !== MMBottomBar.Buttons.BigEdit

    Image {
      source: {
        switch (button) {
        case MMBottomBar.Buttons.Delete: return Style.deleteIcon
        case MMBottomBar.Buttons.EditGeometry: return Style.editIcon
        case MMBottomBar.Buttons.Advanced: return Style.moreIcon
        case MMBottomBar.Buttons.Save: return Style.doneIcon
        case MMBottomBar.Buttons.BigEdit: return Style.doneIcon
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
        case MMBottomBar.Buttons.BigEdit: return qsTr("Edit")
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

    MouseArea {
      anchors.fill: parent
      onClicked: control.clicked(button)
    }
  }
}

