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
          // TODO visual params
          property var length: 20
          property var borderWidth: 5
          property var animationDuration: 1000
          property var primaryColor: InputStyle.fontColor
          property var secondaryColor: InputStyle.fontColorBright

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

            SequentialAnimation on color {
              loops: Animation.Infinite
              ColorAnimation {
                from: cornerItem.primaryColor
                to: cornerItem.secondaryColor
                duration: cornerItem.animationDuration
              }
              ColorAnimation {
                from: cornerItem.secondaryColor
                to: cornerItem.primaryColor
                duration: cornerItem.animationDuration
              }
            }
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

            SequentialAnimation on color {
              loops: Animation.Infinite
              ColorAnimation {
                from: cornerItem.primaryColor
                to: cornerItem.secondaryColor
                duration: cornerItem.animationDuration
              }
              ColorAnimation {
                from: cornerItem.secondaryColor
                to: cornerItem.primaryColor
                duration: cornerItem.animationDuration
              }
            }
          }
        }
      }
    }
  }
}
