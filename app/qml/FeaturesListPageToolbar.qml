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
import "components"

Item {
  id: root
  property bool addButtonVisible: false
  property bool doneButtonVisible: false
  property bool unlinkButtonVisible: false

  height: InputStyle.rowHeightHeader
  width: parent.width
  y: parent.height - height

  signal addButtonClicked
  signal doneButtonClicked
  signal unlinkButtonClicked

  Rectangle {
    anchors.fill: parent
    color: InputStyle.clrPanelBackground
    opacity: InputStyle.panelOpacity

    MouseArea {
      anchors.fill: parent

      onClicked: {
      } // do nothing but do not let click propagate
    }
    RowLayout {
      anchors.bottom: parent.bottom
      height: parent.height
      width: parent.width

      Item {
        Layout.fillWidth: true
        height: parent.height
        visible: addButtonVisible

        MainPanelButton {
          id: addButton
          imageSource: InputStyle.plusIcon
          text: qsTr("Add Feature")
          width: root.height * 0.8

          onActivated: {
            addButtonClicked();
          }
        }
      }
      Item {
        Layout.fillWidth: true
        height: parent.height
        visible: doneButtonVisible

        MainPanelButton {
          id: doneButton
          imageSource: InputStyle.checkIcon
          text: qsTr("Done")
          width: root.height * 0.8

          onActivated: {
            doneButtonClicked();
          }
        }
      }
      Item {
        Layout.fillWidth: true
        height: parent.height
        visible: unlinkButtonVisible

        MainPanelButton {
          id: unlinkButton
          imageSource: InputStyle.unlinkIcon
          text: qsTr("Remove link")
          width: root.height * 0.8

          onActivated: {
            unlinkButtonClicked();
          }
        }
      }
    }
  }
}
