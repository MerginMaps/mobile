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

import "../../app/qmlV2/component"

Column {
  padding: 20
  spacing: 20

  GroupBox {
    title: "MMCheckBox"
    background: Rectangle {
      color: "lightGray"
      border.color: "gray"
    }
    label: Label {
      color: "black"
      text: parent.title
      padding: 5
    }

    Row {
      spacing: 20
      anchors.fill: parent
      MMCheckBox {
        checked: false
        text: checked ? "checked" : "unchecked"
      }
      MMCheckBox {
        checked: true
      }
      MMCheckBox {
        checked: false
        enabled: false
      }
      MMCheckBox {
        checked: true
        enabled: false
      }
    }
  }

  GroupBox {
    title: "MMRadioButton"
    background: Rectangle {
      color: "lightGray"
      border.color: "gray"
    }
    label: Label {
      color: "black"
      text: parent.title
      padding: 5
    }

    Row {
      spacing: 20
      anchors.fill: parent
      MMRadioButton {
        text: "one"
        checked: false
      }
      MMRadioButton {
        text: "two"
        checked: true
      }
      MMRadioButton {
        text: "three"
        enabled: false
        checked: false
      }
    }
  }

  GroupBox {
    title: "MMSwitch"
    background: Rectangle {
      color: "lightGray"
      border.color: "gray"
    }
    label: Label {
      color: "black"
      text: parent.title
      padding: 5
    }

    Row {
      spacing: 20
      anchors.fill: parent
      MMSwitch {
        textOn: "ON"
        textOff: "OFF"
        checked: false
      }
      MMSwitch {
        text: "static"
        checked: true
      }
      MMSwitch {
        text: "disabled"
        enabled: false
        checked: false
      }
    }
  }
}
