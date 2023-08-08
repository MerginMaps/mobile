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
    title: "MMTextArea"
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
      MMTextArea {
        areaHeight: 100
        placeholderText: "Place holder"
      }
      MMTextArea {
        areaHeight: 100
        text: "Disabled"
        enabled: false
      }
      MMTextArea {
        areaHeight: 100
        text: "Something important"
        warningMsg: "Be careful"
      }
      MMTextArea {
        areaHeight: 50
        text: "Something wrong"
        errorMsg: "Error message"
      }
    }
  }
}
