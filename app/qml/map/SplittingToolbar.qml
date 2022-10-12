/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts

import ".."
import "../components"

Item {
  id: root

  signal addClicked
  signal removeClicked
  signal cancelClicked
  signal doneClicked

  property real itemSize: height * 0.8

  focus: true
  height: InputStyle.rowHeightHeader

  Keys.onReleased: function( event ) {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true;
      cancelClicked()
    }
  }

  // background
  Rectangle {
    anchors.fill: parent
    color: InputStyle.clrPanelBackground
    opacity: InputStyle.panelOpacity

    MouseArea {
      anchors.fill: parent
      onClicked: {} // dont do anything, just do not let click event propagate
    }
  }

  // buttons
  RowLayout {
    height: parent.height
    width: parent.width
    anchors.bottom: parent.bottom

    Item {
      Layout.fillWidth: true
      height: parent.height

      MainPanelButton {
        id: removePointButton
        width: root.itemSize
        text: qsTr("Undo")
        imageSource: InputStyle.undoIcon

        onActivated: root.removeClicked()
      }
    }

    Item {
      height: parent.height
      Layout.fillWidth: true

      MainPanelButton {
        id: addButton
        width: root.itemSize
        text: qsTr("Add Point")
        imageSource: InputStyle.plusIcon

        onActivated: root.addClicked()
      }
    }

    Item {
      Layout.fillWidth: true
      height: parent.height

      MainPanelButton {
        id: finishButton
        width: root.itemSize
        text: qsTr("Done")
        imageSource: InputStyle.checkIcon

        onActivated: root.doneClicked()
      }
    }

    Item {
      height: parent.height
      Layout.fillWidth: true

      MainPanelButton {
        id: cancelButton
        width: root.itemSize
        text: qsTr("Cancel")
        imageSource: InputStyle.noIcon

        onActivated: root.cancelClicked()
      }
    }
  }
}
