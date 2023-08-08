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

import "../../../app/qmlV2/component"

Column {
  padding: 20
  spacing: 20

  GroupBox {
    title: "MMComboBox"
    background: Rectangle {
      color: "lightGray"
      border.color: "gray"
    }
    label: Label {
      color: "black"
      text: parent.title
      padding: 5
    }

    Column {
      spacing: 20
      anchors.fill: parent
      MMComboBox {
        model: ["First", "Second", "Third"]
        onActivated: (currentValue) => console.log("Selected " + currentValue)
      }
      MMComboBox {
        model: ["First", "Second", "Third"]
        enabled: false
      }
      MMComboBox {
        model: ["First", "Second", "Third"]
        warningMsg: "Be careful"
      }
      MMComboBox {
        model: ["First", "Second", "Third"]
        errorMsg: "Error message"
      }
    }
  }
}
