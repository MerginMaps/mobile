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


Item {

  signal addButtonClicked()
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

        MainPanelButton {
          id: addButton
          width: root.height * 0.8
          text: qsTr("Add Feature")
          imageSource: "plus.svg"
          onActivated: {
            addButtonClicked()
          }
        }
      }
    }
  }
}
