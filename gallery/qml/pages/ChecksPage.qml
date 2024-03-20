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

import "../../app/qml/account/components" as MMAccountComponents
import "../../app/qml/components" as MMComponents
import "../../app/qml/inputs"

Column {
  padding: 20
  spacing: 20

  GroupBox {
    title: "MMComponents.MMCheckBox"
    width: 200
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
      MMComponents.MMCheckBox {
        checked: false
        text: checked ? "checked" : "unchecked"
      }
      MMComponents.MMCheckBox {
        checked: true
      }
      MMComponents.MMCheckBox {
        checked: false
        enabled: false
      }
      MMComponents.MMCheckBox {
        checked: true
        enabled: false
      }
    }
  }

  GroupBox {
    title: "MMIconCheckBoxHorizontal"
    background: Rectangle {
      color: __style.lightGreenColor
      border.color: "gray"
    }
    label: Label {
      color: "black"
      text: parent.title
      padding: 5
    }

    Row {
      spacing: 10
      anchors.fill: parent

      MMAccountComponents.MMIconCheckBoxHorizontal {
        checked: false
        sourceIcon: __style.qgisIcon
        text: "QGIS website"
      }

      MMAccountComponents.MMIconCheckBoxHorizontal {
        checked: true
        sourceIcon: __style.qgisIcon
        text: "QGIS website"
      }

      MMAccountComponents.MMIconCheckBoxHorizontal {
        checked: false
        sourceIcon: __style.redditIcon
        text: "Reddit"
        small: true
      }

      MMAccountComponents.MMIconCheckBoxHorizontal {
        checked: true
        sourceIcon: __style.redditIcon
        text: "Reddit"
        small: true
      }
    }
  }

  GroupBox {
    title: "MMIconCheckBoxVertical"
    background: Rectangle {
      color: __style.lightGreenColor
      border.color: "gray"
    }
    label: Label {
      color: "black"
      text: parent.title
      padding: 5
    }

    Row {
      spacing: 10
      anchors.fill: parent

      MMAccountComponents.MMIconCheckBoxVertical {
        checked: false
        sourceIcon: __style.archaeologyIcon
        text: "Archeology"
        bgColorIcon: __style.sandColor
      }

      MMAccountComponents.MMIconCheckBoxVertical {
        checked: true
        sourceIcon: __style.tractorIcon
        text: "Agriculture Long text"
        bgColorIcon: __style.sunColor
      }
    }
  }

  GroupBox {
    title: "MMComponents.MMRadioButton"
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
      MMComponents.MMRadioButton {
        text: "one"
        checked: false
      }
      MMComponents.MMRadioButton {
        text: "two"
        checked: true
      }
      MMComponents.MMRadioButton {
        text: "three"
        enabled: false
        checked: false
      }
    }
  }

  GroupBox {
    title: "MMComponents.MMSwitch"
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
      MMComponents.MMSwitch {
        checked: false
        text: checked ? "ON" : "OFF"
      }
      MMComponents.MMSwitch {
        text: "static"
        checked: true
      }
      MMComponents.MMSwitch {
        text: "disabled"
        enabled: false
        checked: false
      }
    }
  }
}
