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

ScrollView {
  anchors.fill: parent
  Column {
    width: parent.width
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

            enabled: false

            onClicked: text = (text === "Clicked" ? "Primary" : "Clicked")
          }
        }

        MMButton {

          width: parent.width

          fontColor: __style.grapeColor
          iconColor: __style.grapeColor
          bgndColor: __style.negativeColor
          fontColorHover: __style.negativeColor
          iconColorHover: __style.negativeColor
          bgndColorHover: __style.grapeColor

          text: "Primary custom color with icon"

          iconSourceRight: __style.arrowLinkRightIcon
          iconSourceLeft: __style.uploadIcon
        }

        Row {
          width: parent.width
          spacing: 20

          MMButton {
            text: "Primary flex width (no witdth set)"
          }

          MMButton {
            text: "Small Primary"
            size: MMButton.Sizes.Small
          }

          MMButton {
            text: "Small Primary with Icon"
            iconSourceLeft: __style.syncIcon
            size: MMButton.Sizes.Small
          }
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
            enabled: false

            onClicked: text = (text === "Clicked" ? "Secondary" : "Clicked")
          }
        }

        MMButton {

          width: parent.width

          fontColor: __style.grapeColor
          iconColor: __style.grapeColor
          bgndColor: __style.negativeColor
          fontColorHover: __style.negativeColor
          iconColorHover: __style.negativeColor
          bgndColorHover: __style.grapeColor

          type: MMButton.Types.Secondary
          text: "Secondary custom color with icon"

          iconSourceRight: __style.arrowLinkRightIcon
        }

        Row {
          width: parent.width
          spacing: 20

          MMButton {
            type: MMButton.Types.Secondary
            text: "Secondary flex width (no witdth set)"
            iconSourceLeft: __style.uploadIcon
            iconSourceRight: __style.uploadIcon
          }

          MMButton {
            text: "Small Secondary"
            type: MMButton.Types.Secondary
            size: MMButton.Sizes.Small
          }

          MMButton {
            text: "Small Secondary with Icon"
            type: MMButton.Types.Secondary
            iconSourceLeft: __style.syncIcon
            size: MMButton.Sizes.Small
          }
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
            enabled: false

            iconSourceLeft: __style.uploadIcon

            onClicked: text = (text === "Clicked" ? "Tertiary" : "Clicked")
          }
        }

        MMButton {

          width: parent.width

          fontColor: __style.grapeColor
          iconColor: __style.grapeColor
          fontColorHover: __style.negativeColor
          iconColorHover: __style.negativeColor

          type: MMButton.Types.Tertiary
          text: "Tertiary custom color with icon"

          iconSourceRight: __style.arrowLinkRightIcon
        }

        Row {
          width: parent.width
          spacing: 20

          MMButton {
            type: MMButton.Types.Tertiary
            text: "Tertiary flex width (no witdth set)"
            fontColor: __style.nightColor
            iconSourceLeft: __style.globeIcon
          }

          MMButton {
            text: "Small Tertiary"
            type: MMButton.Types.Tertiary
            size: MMButton.Sizes.Small
          }

          MMButton {
            text: "Small Tertiary with Icon"
            type: MMButton.Types.Tertiary
            iconSourceLeft: __style.syncIcon
            size: MMButton.Sizes.Small
          }
        }
      }
    }

    GroupBox {
      title: "MMRoundButton"
      background: Rectangle {
        color: "lightgray"
        border.color: "black"
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
          iconSource: __style.closeIcon
        }

        MMRoundButton {
          iconSource: __style.checkmarkIcon
          iconColor: __style.grapeColor
          bgndColor: __style.negativeColor
        }

        MMRoundButton {
          iconSource: __style.checkmarkIcon
          iconColor: __style.forestColor
          bgndColor: __style.grassColor
        }

      }
    }
  }
}
