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

    width: parent.width - 2 * parent.padding

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
      width: parent.width

      spacing: 10

      MMText {
        text: "1. Primary"
        width: parent.width
      }

      Row {

        width: parent.width
        spacing: 20

        MMButton {
          text: "Primary"

          width: parent.width / 2 - parent.spacing

          onClicked: text = (text === "Clicked" ? "Primary" : "Clicked")
        }

        MMButton {
          text: "Disabled"

          width: parent.width / 2 - parent.spacing

          disabled: true

          onClicked: text = (text === "Clicked" ? "Primary" : "Clicked")
        }
      }

      MMButton {

        width: parent.width

        fontColor: __style.grapeColor
        bgndColor: __style.negativeColor
        fontColorHover: __style.negativeColor
        bgndColorHover: __style.grapeColor

        text: "Primary custom color with icon"

        iconSource: __style.arrowLinkRightIcon
      }

      MMButton {
        text: "Primary flex width (no witdth set)"
      }

      MMListSpacer { height: __style.margin20 }

      MMText {
        text: "2. Secondary"
        width: parent.width
      }

      Row {

        width: parent.width
        spacing: 20

        MMButton {
          text: "Secondary"

          width: parent.width / 2 - parent.spacing

          type: MMButton.Types.Secondary

          onClicked: text = (text === "Clicked" ? "Secondary" : "Clicked")
        }

        MMButton {
          text: "Disabled"

          width: parent.width / 2 - parent.spacing

          type: MMButton.Types.Secondary
          disabled: true

          onClicked: text = (text === "Clicked" ? "Secondary" : "Clicked")
        }
      }

      MMButton {

        width: parent.width

        fontColor: __style.grapeColor
        bgndColor: __style.negativeColor
        fontColorHover: __style.negativeColor
        bgndColorHover: __style.grapeColor

        type: MMButton.Types.Secondary
        text: "Secondary custom color with icon"

        iconSource: __style.arrowLinkRightIcon
      }

      MMButton {
        type: MMButton.Types.Secondary
        text: "Secondary flex width (no witdth set)"
      }

      MMListSpacer { height: __style.margin20 }

      MMText {
        text: "3. Tertiary"
        width: parent.width
      }

      Row {

        width: parent.width
        spacing: 20

        MMButton {
          text: "Tertiary"

          width: parent.width / 2 - parent.spacing

          type: MMButton.Types.Tertiary

          onClicked: text = (text === "Clicked" ? "Tertiary" : "Clicked")
        }

        MMButton {
          text: "Disabled"

          width: parent.width / 2 - parent.spacing

          type: MMButton.Types.Tertiary
          disabled: true

          onClicked: text = (text === "Clicked" ? "Tertiary" : "Clicked")
        }
      }

      MMButton {

        width: parent.width

        fontColor: __style.grapeColor
        fontColorHover: __style.negativeColor

        type: MMButton.Types.Tertiary
        text: "Tertiary custom color with icon"

        iconSource: __style.arrowLinkRightIcon
      }

      MMButton {
        type: MMButton.Types.Tertiary
        text: "Tertiary flex width (no witdth set)"
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
}
