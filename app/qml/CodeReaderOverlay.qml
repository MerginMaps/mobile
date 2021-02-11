import QtQuick 2.7
import QtQuick.Layouts 1.3

Item {
  id: root
  property real rectSize

  Item {
    id: captureZoneCorners
    anchors.centerIn: root
    width: root.rectSize
    height: root.rectSize

    GridLayout {
      columns: 2
      width: root.rectSize
      height: root.rectSize

      Repeater {
        model: [0, 1, 3, 2]

        delegate: Item {
          property var length: 20 // TODO
          property var borderWidth: 5 // TODO

          id: cornerItem
          Layout.fillHeight: true
          Layout.fillWidth: true
          rotation: modelData * 90

          Rectangle {
            anchors {
              top: parent.top
              left: parent.left
            }

            width: cornerItem.length
            height: cornerItem.borderWidth
            color: InputStyle.fontColor
            radius: width / 2
          }

          Rectangle {
            anchors {
              top: parent.top
              left: parent.left
            }

            width: cornerItem.borderWidth
            height: cornerItem.length
            color: InputStyle.fontColor
            radius: width / 2
          }
        }
      }
    }
  }
}
