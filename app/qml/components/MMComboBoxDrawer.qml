/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import "."

Drawer {
  id: control

  property alias title: title.text
  property alias model: listView.model

  padding: 20 * __dp

  signal clicked(type: string)

  width: ApplicationWindow.window.width
  height: mainColumn.height
  edge: Qt.BottomEdge

  Rectangle {
    color: roundedRect.color
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    height: 2 * radius
    anchors.topMargin: -radius
    radius: 20 * __dp
  }

  Rectangle {
    id: roundedRect

    anchors.fill: parent
    color: __style.whiteColor

    Column {
      id: mainColumn

      width: parent.width
      spacing: 20 * __dp
      leftPadding: control.padding
      rightPadding: control.padding
      bottomPadding: control.padding

      Row {
        width: parent.width - 2 * control.padding
        anchors.horizontalCenter: parent.horizontalCenter

        Item { width: closeButton.width; height: 1 }

        Text {
          id: title

          anchors.verticalCenter: parent.verticalCenter
          font: __style.t2
          width: parent.width - closeButton.width * 2
          color: __style.forestColor
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideRight
        }

        Image {
          id: closeButton

          source: __style.closeButtonIcon

          MouseArea {
            anchors.fill: parent
            onClicked: control.visible = false
          }
        }
      }

      ListView {
        id: listView

        width: parent.width - 2 * control.padding
        height: control.model ? control.model.count * __style.menuDrawerHeight : 0
        interactive: false

        delegate: Item {
          id: delegate

          //signal clicked(type: string)

          width: listView.width
          height: __style.menuDrawerHeight

          Rectangle {
            anchors.top: parent.top
            width: parent.width
            height: 1 * __dp
            color: __style.grayColor
          }

          Row {
            height: parent.height
            width: parent.width
            spacing: 10 * __dp

            Text {
              height: parent.height
              verticalAlignment: Text.AlignVCenter
              text: model.FeatureTitle
              color: __style.nightColor
              font: __style.t3
            }
          }

          MouseArea {
            anchors.fill: parent
            //onClicked: control.clicked(control.type)
          }
        }

      }
    }
  }
}
