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
    title: "MMProgressBar"
    background: Rectangle {
      color: "white"
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
      MMProgressBar {
        position: 0
      }
      MMProgressBar {
        position: 0.4
      }
    }
  }

  GroupBox {
    title: "MMHeader"
    background: Rectangle {
      color: "white"
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
      MMHeader {
        headerTitle: "With Progress"
        step: 1
      }

      MMHeader {
        headerTitle: "No Back"
        backVisible: false
        step: 2
      }
    }
  }

  GroupBox {
    title: "MMHlineText"
    background: Rectangle {
      color: "white"
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
      MMHlineText {
        title: "My text is great"
      }
    }
  }

  GroupBox {
    title: "MMTextBubble"
    background: Rectangle {
      color: "gray"
    }
    label: Label {
      color: "black"
      text: parent.title
      padding: 5
    }

    Column {
      spacing: 20
      anchors.fill: parent
      MMTextBubble {
        title: "My text is great"
        description: "My text is great. y text is great My text is great. y text is great. y text is great"
      }
    }
  }
}
