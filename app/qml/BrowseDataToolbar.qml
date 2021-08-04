/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.0
import QtQuick.Layouts 1.3
import "./components"

Item {

  signal addButtonClicked()
  signal doneButtonClicked()
  signal unlinkButtonClicked()

  property bool addButtonVisible: false
  property bool doneButtonVisible: false
  property bool unlinkButtonVisible: false

  id: root

  height: InputStyle.rowHeightHeader
  width: parent.width
  y: parent.height - height

  Rectangle {
    anchors.fill: parent
    color: InputStyle.clrPanelBackground
    opacity: InputStyle.panelOpacity

    MouseArea {
      anchors.fill: parent
      onClicked: {} // do nothing but do not let click propagate
    }

    RowLayout {
      height: parent.height
      width: parent.width
      anchors.bottom: parent.bottom

      Item {
        height: parent.height
        Layout.fillWidth: true
        visible: addButtonVisible

        MainPanelButton {
          id: addButton
          width: root.height * 0.8
          text: qsTr("Add Feature")
          imageSource: InputStyle.plusIcon
          onActivated: {
            addButtonClicked()
          }
        }
      }

      Item {
        height: parent.height
        Layout.fillWidth: true
        visible: doneButtonVisible

        MainPanelButton {
          id: doneButton
          width: root.height * 0.8
          text: qsTr("Done")
          imageSource: InputStyle.checkIcon
          onActivated: {
            doneButtonClicked()
          }
        }
      }

      Item {
        height: parent.height
        Layout.fillWidth: true
        visible: unlinkButtonVisible

        MainPanelButton {
          id: unlinkButton
          width: root.height * 0.8
          text: qsTr("Remove link")
          imageSource: InputStyle.unlinkIcon
          onActivated: {
            unlinkButtonClicked()
          }
        }
      }
    }
  }
}
