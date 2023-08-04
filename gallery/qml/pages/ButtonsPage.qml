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
import QtQuick.Layouts

import "../../../app/qmlV2/component"

Column {
  padding: 20
  spacing: 20

  GroupBox {
    title: "MMButton"
    background: Rectangle {
      color: "white"
      border.color: "gray"
    }
    label: Label {
      color: "black"
      text: parent.title
      padding: 5
    }

    RowLayout {
      anchors.fill: parent
      MMButton {
        text: "Primary"
        onClicked: text = (text === "Clicked" ? "Primary" : "Clicked")
      }
      MMButton {
        text: "Disabled"
        enabled: false
      }
    }
  }

  GroupBox {
    title: "MMLinkButton"
    background: Rectangle {
      color: "white"
      border.color: "gray"
    }
    label: Label {
      color: "black"
      text: parent.title
      padding: 5
    }

    RowLayout {
      anchors.fill: parent
      MMLinkButton {
        text: "Secondary"
        onClicked: text = (text === "Clicked" ? "Secondary" : "Clicked")
      }
      MMLinkButton {
        text: "Disabled"
        enabled: false
      }
    }
  }

  GroupBox {
    title: "MMLink todo..."
    background: Rectangle {
      color: "white"
      border.color: "gray"
    }
    label: Label {
      color: "black"
      text: parent.title
      padding: 5
    }

    RowLayout {
      anchors.fill: parent
      MMLink {
        text: "Tertriary"
        onClicked: text = (text === "Clicked" ? "Tertriary" : "Clicked")
      }
      MMLink {
        text: "Disabled"
        enabled: false
      }
    }
  }
}
