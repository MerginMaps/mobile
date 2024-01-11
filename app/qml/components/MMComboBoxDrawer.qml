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
  property alias dropDownTitle: title.text
  property bool multiSelect: false

  padding: 20 * __dp

  signal clicked(type: string)

  width: ApplicationWindow.window.width
  height: (mainColumn.height > ApplicationWindow.window.height ? ApplicationWindow.window.height : mainColumn.height) - 20 * __dp
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

        bottomMargin: primaryButton.height + 20 * __dp
        width: parent.width - 2 * control.padding
        height: {
          if(control.model.count > 4)
            return ApplicationWindow.window.height - 100 * __dp
          if(control.model)
            return (1 + control.model.count) * __style.menuDrawerHeight
          return 0
        }
        clip: true

        delegate: Item {
          id: delegate

          property bool checked: root.multiSelect ? false : listView.currentIndex === model.index

          width: listView.width
          height: __style.menuDrawerHeight

          Rectangle {
            anchors.top: parent.top
            width: parent.width
            height: 1 * __dp
            color: __style.grayColor
            visible: model.index
          }

          Row {
            height: parent.height
            width: parent.width
            spacing: 10 * __dp

            Text {
              width: parent.width - icon.width - parent.spacing
              height: parent.height
              verticalAlignment: Text.AlignVCenter
              text: model.FeatureTitle
              color: __style.nightColor
              font: __style.t3
              elide: Text.ElideRight
            }

            MMIcon {
              id: icon
              height: parent.height
              width: 20 * __dp
              color: __style.forestColor
              source: __style.comboBoxCheckIcon
              visible: delegate.checked
            }
          }

          MouseArea {
            anchors.fill: parent
            onClicked: {
              if(root.multiSelect)
                delegate.checked = !delegate.checked
              listView.currentIndex = model.index
            }
          }
        }
      }
    }

    MMButton {
      id: primaryButton

      width: parent.width - 2 * 20 * __dp
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: parent.bottom
      anchors.bottomMargin: 20 * __dp

      text: qsTr("Confirm selection")

      onClicked: {
        for(let i=0; i<listView.model.count; i++)
          console.log(i + " " + listView.itemAtIndex(i).checked)
        close()
      }
    }
  }
}
