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
import "../../app/qml/gps"
import "../../app/qml/gps/components" as MMGpsComponents

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
      title: "MMPageHeader"

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

        MMPageHeader {
          title: "Only title without anything on left nor right"
          width: parent.width

          backVisible: false

          Rectangle { anchors.fill: parent; color: "red"; opacity: .3 }
        }

        MMPageHeader {
          title: "Title with back button"
          width: parent.width

          backVisible: true
        }

      }
    }

    GroupBox {
      title: "MMDrawerHeader"
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

        MMDrawerHeader {
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
      title: "MMBusyIndicator"
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
        MMBusyIndicator {
          running: true
        }
      }
    }

    GroupBox {
      title: "MMLine"
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
        MMLine {
          width: page.width - 64
        }
      }
    }

    GroupBox {
      title: "MMInfoBox"
      background: Rectangle {
        color: __style.lightGreenColor
      }
      label: Label {
        color: "black"
        text: parent.title
        padding: 5
      }

      Column {
        spacing: 20
        anchors.fill: parent

        MMNotificationBox
        {
          width: page.width - 64
          type: MMNotificationBox.Warning
          title: "Your payment is not working"
          description: "We would like to inform you that MerginMaps will undergo scheduled maintance on 1st of october 2025. As a result of this upgrade you will be able to see better performance on our servers."

          buttonText: "Upgrade Now"
          onButtonClicked: console.log("No workspaces")
        }

        MMNotificationBox
        {
          width: page.width - 64
          type: MMNotificationBox.Error
          title: "Your payment is not working"
          description: "We would like to inform you that MerginMaps will undergo scheduled maintance on 1st of october 2025. As a result of this upgrade you will be able to see better performance on our servers."

          buttonText: "Service Cancelled"
          onButtonClicked: console.log("No workspaces")
        }

        MMNotificationBox
        {
          width: page.width - 64
          type: MMNotificationBox.Error
          title: "Your payment is not working"
          description: ""
          buttonText: "Service Cancelled"
          onButtonClicked: console.log("Empty description")
        }

        MMInfoBox {
          width: page.width - 64
          title: "A tip from us"
          description: "A good candidate for a workspace name is the name of your team or organisation"
          imageSource: __style.bubbleImage
        }

        MMInfoBox {
          width: page.width - 64
          title: "Server is broken, sorry"
          description: "Please wait for tomorrow"

          color: __style.nightColor
          textColor: __style.polarColor

          imageSource: __style.warnLogoImage
        }

        MMInfoBox {
          width: page.width - 64
          title: "Info box with a link!"
          linkText: "Learn more"

          imageSource: __style.noWorkspaceImage

          onClicked: console.log("No workspaces brooo")
        }
      }
    }

    GroupBox {
      title: "MMAvatarLettersItem"
      background: Rectangle {
        color: "white"
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
        MMAvatarLettersItem {
          abbrv: "PP"
        }

        MMAvatarLettersItem {
          abbrv: "JP"
          hasNotification: true
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

          MMGpsComponents.MMGpsDataText {
            title: "Gps Data Title"
            value: "Gps Data Description"
          }

          MMGpsComponents.MMGpsDataText {
            title: "Gps Data Right Title"
            value: "Gps Data Right Description"
            alignmentRight: true
          }
        }
      }
    }
  }
}
