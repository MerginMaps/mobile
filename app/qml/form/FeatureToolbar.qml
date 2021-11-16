/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.14
import QtQuick.Controls 2.14
import "../"
import "../components"

Item {
  id: toolbar
  property bool isFeaturePoint: false
  property int itemSize: toolbar.height * 0.8

  signal deleteClicked
  signal editClicked
  signal editGeometryClicked

  Rectangle {
    anchors.fill: parent
    color: InputStyle.clrPanelBackground
  }
  Row {
    id: readOnlyRow
    anchors.fill: parent
    height: parent.height
    width: parent.width

    Item {
      height: parent.height
      width: parent.width / parent.children.length

      MainPanelButton {
        id: openProjectBtn
        imageSource: InputStyle.editIcon
        text: qsTr("Edit")
        width: toolbar.itemSize

        onActivated: {
          toolbar.editClicked();
        }
      }
    }
  }
  Row {
    id: editRow
    anchors.fill: parent
    height: parent.height
    width: parent.width

    Item {
      height: parent.height
      width: parent.width / parent.children.length

      MainPanelButton {
        imageSource: InputStyle.removeIcon
        text: qsTr("Delete")
        width: toolbar.itemSize

        onActivated: {
          toolbar.deleteClicked();
        }
      }
    }
    Item {
      height: parent.height
      width: parent.width / parent.children.length

      MainPanelButton {
        enabled: isFeaturePoint
        imageSource: InputStyle.editIcon
        text: qsTr("Edit geometry")
        width: toolbar.itemSize

        onActivated: {
          toolbar.editGeometryClicked();
        }
      }
    }
  }

  states: [
    State {
      name: "edit"

      PropertyChanges {
        target: editRow
        visible: true
      }
      PropertyChanges {
        target: readOnlyRow
        visible: false
      }
    },
    State {
      name: "add"

      PropertyChanges {
        target: editRow
        visible: true
      }
      PropertyChanges {
        target: readOnlyRow
        visible: false
      }
    },
    State {
      name: "readOnly"

      PropertyChanges {
        target: editRow
        visible: false
      }
      PropertyChanges {
        target: readOnlyRow
        visible: true
      }
    }
  ]
}
