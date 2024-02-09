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
import "../../app/qml/misc"

ScrollView {
  id: page

  Column {
    id: mainColumn

    anchors {
      fill: parent
      margins: __style.pageMargins
    }

    spacing: 5

    Row {
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
            position: 0.6
          }
        }
      }

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
            width: 60 * __dp
            height: 4 * __dp

            position: 0
            color: __style.grassColor
            progressColor: __style.forestColor
          }
          MMProgressBar {
            width: 60 * __dp
            height: 4 * __dp

            position: 0.6
            color: __style.grassColor
            progressColor: __style.forestColor
          }
        }
      }
    }

    GroupBox {
      title: "MMHeader"

      width: parent.width

      background: Rectangle {
        color: __style.lightGreenColor
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
          title: "Only title without anything on left nor right"
          width: parent.width

          backVisible: false

          Rectangle { anchors.fill: parent; color: "red"; opacity: .3 }
        }

        MMHeader {
          title: "Title with back button"
          width: parent.width

          backVisible: true
        }

        MMHeader {
          title: "Title with progress bar"
          width: parent.width

          backVisible: false
          rightMarginShift: progressBar.width

          MMProgressBar {
            id: progressBar

            anchors.right: parent.right
            anchors.rightMargin: __style.pageMargins
            anchors.verticalCenter: parent.verticalCenter

            width: 60 * __dp
            height: 4 * __dp

            color: __style.grassColor
            progressColor: __style.forestColor
            position: 0.5
          }
        }

        MMHeader {
          title: "Title with back button and Progress bar"
          width: parent.width
          backVisible: true

          rightMarginShift: progressBar2.width

          MMProgressBar {
            id: progressBar2

            anchors.right: parent.right
            anchors.rightMargin: __style.pageMargins
            anchors.verticalCenter: parent.verticalCenter

            width: 60 * __dp
            height: 4 * __dp

            color: __style.grassColor
            progressColor: __style.forestColor
            position: 0.9
          }
        }
      }
    }

    GroupBox {
      title: "MMHeader (bigger title)"
      width: page.width - 40

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
          title: "Drawer title"
          titleFont: __style.h3
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
          width: page.width - 64
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
          width: page.width - 64
          title: "Tip from Mergin Maps"
          description: "A good candidate for a workspace name is the name of your team or organisation"
        }
      }
    }

    GroupBox {
      title: "MMWarningBubble"
      background: Rectangle {
        color: "white"
      }
      label: Label {
        color: "black"
        text: parent.title
        padding: 5
      }

      Column {
        spacing: 20
        anchors.fill: parent
        MMWarningBubble {
          width: page.width - 64
          text: "Server is broken, sorry. \n Please wait for tomorrow"
        }
      }
    }

    GroupBox {
      width: page.width - 40
      title: "MMGpsDataText"
      background: Rectangle {
        color: "white"
      }
      label: Label {
        color: "black"
        text: parent.title
        padding: 5
      }

      Column {
        spacing: 20
        anchors.fill: parent

        Row {
          width: parent.width
          height: 67 * __dp

          MMGpsDataText {
            titleText: "Gps Data Title"
            descriptionText: "Gps Data Description"
          }

          MMGpsDataText {
            titleText: "Gps Data Right Title"
            descriptionText: "Gps Data Right Description"
            alignmentRight: true
          }
        }

        MMLine {}
      }
    }
  }
}
