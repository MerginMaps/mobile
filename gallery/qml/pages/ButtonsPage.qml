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

import "../../app/qml/components"

Column {
  padding: 20
  spacing: 5

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

    Row {
      spacing: 20
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

    Row {
      spacing: 20
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
    title: "MMLink"
    background: Rectangle {
      color: "white"
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
      MMLink {
        text: "Tertriary"
        width: 150
        rightIcon: __style.arrowLinkRightIcon
        onClicked: text = (text === "Clicked" ? "Tertriary" : "Clicked")
      }
      MMLink {
        text: "Disabled"
        rightIcon: __style.arrowLinkRightIcon
        width: 150
        enabled: false
      }
    }
  }

  GroupBox {
    title: "MMRoundButton"
    background: Rectangle {
      color: "white"
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
      MMRoundButton {
      }
      MMRoundButton {
        enabled: false
      }
    }
  }

  GroupBox {
    title: "MMRoundLinkButton"
    background: Rectangle {
      color: "white"
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
      MMRoundLinkButton {
      }
      MMRoundLinkButton {
        enabled: false
      }
    }
  }

  GroupBox {
    title: "MMBackButton"
    background: Rectangle {
      color: "white"
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

      MMBackButton {
        enabled: false
      }
    }
  }
}
