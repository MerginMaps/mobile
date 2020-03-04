import QtQuick 2.0

import QgsQuick 0.1 as QgsQuick
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3

Rectangle {
  id: positionInformationView
  property var positionKit
  property var positionMarker
  property var dp: QgsQuick.Utils.dp
  property double rowHeight: 30*dp
  property double antennaHeight: NaN
  color: "yellow"

  height: grid.rows * positionInformationView.rowHeight
  width: parent.width
  anchors.margins: 20

  Grid {
    id: grid
    flow: GridLayout.TopToBottom
    rows: parent.width > 1000*dp ? 1 : parent.width > 620*dp ? 2 : 3
    width: parent.width
    property double cellWidth: grid.width / ( 6 / grid.rows )

    Rectangle {
      id: x
      height: rowHeight
      width: grid.cellWidth
      color: "#e6f2fd"

      Text {
        anchors.margins:  10*dp
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        text: "X " + Number(positionKit.projectedPosition.x)
      }
    }

    Rectangle {
      height: rowHeight
      width: grid.cellWidth
      color: "white"

      Text {
        anchors.margins:  10*dp
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        text: "Y " + Number(positionKit.projectedPosition.y)
      }
    }

    Rectangle {
      height: rowHeight
      width: grid.cellWidth
      color: grid.rows === 2 ? "white" : "#e6f2fd"

      Text {
        anchors.margins:  10*dp
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        text: "Dir: " + Number(positionKit.direction)
      }
    }

    Rectangle {
      height: rowHeight
      width: grid.cellWidth
      color: grid.rows === 2 ? "#e6f2fd" : "white"

      Text {
        anchors.margins:  10*dp
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        text: "Acc: " + Number(positionKit.accuracy)
      }
    }

    Rectangle {
      height: rowHeight
      width: grid.cellWidth
      color: "#e6f2fd"

      Text {
        anchors.margins:  10*dp
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
         text: "DirM: " + Number(positionMarker.direction)
      }
    }

    Rectangle {
      height: rowHeight
      width: grid.cellWidth
      color: "white"

      Text {
        anchors.margins:  10*dp
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        text: "SpeedM: " + Number(positionMarker.groundSpeed)
      }
    }
  }
}
