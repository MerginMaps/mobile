import QtQuick

Item {
  id: root

  property rect captureRect

  Item {
    id: captureZoneCorners

    x: root.captureRect.x
    y: root.captureRect.y

    width: root.captureRect.width
    height: root.captureRect.height

    Rectangle {
      id: topLeftCornerH

      anchors {
        top: parent.top
        left: parent.left
      }

      width: 20
      height: 5

      color: InputStyle.fontColor
      radius: height / 2
    }

    Rectangle {
      id: topLeftCornerV

      anchors {
        top: parent.top
        left: parent.left
      }

      width: 5
      height: 20

      color: InputStyle.fontColor
      radius: width / 2
    }

    Rectangle {
      id: bottomLeftCornerH

      anchors {
        bottom: parent.bottom
        left: parent.left
      }

      width: 20
      height: 5

      color: InputStyle.fontColor
      radius: height / 2
    }

    Rectangle {
      id: bottomLeftCornerV

      anchors {
        bottom: parent.bottom
        left: parent.left
      }

      width: 5
      height: 20

      color: InputStyle.fontColor
      radius: width / 2
    }

    Rectangle {
      id: topRightCornerH

      anchors {
        top: parent.top
        right: parent.right
      }

      width: 20
      height: 5

      color: InputStyle.fontColor
      radius: height / 2
    }

    Rectangle {
      id: topRightCornerV

      anchors {
        top: parent.top
        right: parent.right
      }

      width: 5
      height: 20

      color: InputStyle.fontColor
      radius: width / 2
    }

    Rectangle {
      id: bottomRightCornerH

      anchors {
        bottom: parent.bottom
        right: parent.right
      }

      width: 20
      height: 5

      color: InputStyle.fontColor
      radius: height / 2
    }

    Rectangle {
      id: bottomRightCornerV

      anchors {
        bottom: parent.bottom
        right: parent.right
      }

      width: 5
      height: 20

      color: InputStyle.fontColor
      radius: width / 2
    }

    Rectangle {
      id: scanIndicator

      anchors {
        horizontalCenter: parent.horizontalCenter
      }

      width: parent.width
      height: 1

      color: InputStyle.fontColor

      SequentialAnimation {
        id: scanIndicatorAnimation

        loops: Animation.Infinite

        PropertyAnimation {
          id: toTopAnimation

          target: scanIndicator
          property: "y"
          duration: 2000
        }

        PropertyAnimation {
          id: toBottomAnimation

          target: scanIndicator
          property: "y"
          duration: 2000
        }
      }
    }
  }

  onCaptureRectChanged: function( captureRect ) {
    toTopAnimation.to = 5
    toBottomAnimation.to = captureRect.height - 10
    scanIndicatorAnimation.start()
  }
}
