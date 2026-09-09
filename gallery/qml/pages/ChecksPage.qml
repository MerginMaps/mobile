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

ScrollView {
  id: scrollView
  anchors.fill: parent
  contentWidth: availableWidth

  // Rectangle {
  //   border.color: "red"
  //   anchors.fill: column
  // }

  Column {
    id: column
    width: scrollView.availableWidth
    padding: 20
    spacing: 20

    GroupBox {
      title: "MMComponents.MMCheckBox"
      width: parent.width - parent.leftPadding - parent.rightPadding
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
      title: "MMCheckBoxHorizontal"
      width: parent.width - parent.leftPadding - parent.rightPadding
      background: Rectangle {
        color: __style.lightGreenColor
        border.color: "gray"
        width: parent.width
      }
      label: Label {
        color: "black"
        text: parent.title
        padding: 5
      }

      Flow {
        spacing: 10
        anchors.fill: parent

        MMComponents.MMCheckBoxHorizontal {
          checked: false
          text: "Bluetooth provider"
        }

        MMComponents.MMCheckBoxHorizontal {
          checked: true
          text: "Bluetooth provider"
        }

        MMComponents.MMCheckBoxHorizontal{
          checked: false
          text: "Network provider"
          small: true
        }

        MMComponents.MMCheckBoxHorizontal{
          checked: true
          text: "Network provider"
          small: true
        }

        MMComponents.MMCheckBoxHorizontal{
          checked: true
          width: 300 * __dp
          text: "Trimble provider"
          description: "Trimble receivers via Trimble Mobile Manager"
        }


        MMComponents.MMCheckBoxHorizontal{
          checked: false
          width: 300 * __dp
          text: "Trimble provider"
          description: "Trimble receivers via Trimble Mobile Manager"
          showBorder: true
        }
      }
    }

    GroupBox {
      title: "MMIconCheckBoxHorizontal"
      width: parent.width - parent.leftPadding - parent.rightPadding
      background: Rectangle {
        color: __style.lightGreenColor
        border.color: "gray"
      }
      label: Label {
        color: "black"
        text: parent.title
        padding: 5
      }

      Flow {
        spacing: 10
        anchors.fill: parent

        MMComponents.MMIconCheckBoxHorizontal{
          checked: false
          sourceIcon: __style.qgisIcon
          text: "QGIS website"
        }

        MMComponents.MMIconCheckBoxHorizontal{
          checked: true
          sourceIcon: __style.qgisIcon
          text: "QGIS website"
        }

        MMComponents.MMIconCheckBoxHorizontal{
          checked: false
          sourceIcon: __style.redditIcon
          text: "Reddit"
          small: true
        }

        MMComponents.MMIconCheckBoxHorizontal{
          checked: true
          width: 300 * __dp
          sourceIcon: __style.redditIcon
          text: "Reddit"
          description: "This is a small description to check the functionality of this component"
          small: false
        }


        MMComponents.MMIconCheckBoxHorizontal{
          checked: false
          width: 300 * __dp
          sourceIcon: __style.redditIcon
          text: "Internet provider mock"
          description: "This is a small description to check the functionality of this component"
          small: false
          showBorder: true
        }
      }
    }

    GroupBox {
      title: "MMIconCheckBoxVertical"
      width: parent.width - parent.leftPadding - parent.rightPadding
      background: Rectangle {
        color: __style.lightGreenColor
        border.color: "gray"
      }
      label: Label {
        color: "black"
        text: parent.title
        padding: 5
      }

      Flow {
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
      width: parent.width - parent.leftPadding - parent.rightPadding
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
      title: "MMComponents.MMSegmentControl"
      width: parent.width - parent.leftPadding - parent.rightPadding
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
        spacing: 10
        anchors.fill: parent

        MMComponents.MMSegmentControl {}

        MMComponents.MMSegmentControl {
          selectedIndex: MMComponents.MMSegmentControl.Options.True
        }

        MMComponents.MMSegmentControl {
          selectedIndex: MMComponents.MMSegmentControl.Options.False
          onSelectedIndexChanged: { console.log( "selected:", selectedIndex ) }
        }

        MMComponents.MMSegmentControl {
          enabled: false
        }

        MMComponents.MMSegmentControl {
          allText: qsTr( "No filter" )
          trueText: qsTr( "Inspected" )
          falseText: qsTr( "Not inspected" )
          selectedIndex: MMComponents.MMSegmentControl.Options.False
        }

        MMComponents.MMSegmentControl {
          allText: qsTr( "A very long label that should be elided" )
          trueText: qsTr( "Another extremely long true label" )
          falseText: qsTr( "A different very long false label" )
        }
      }
    }

    GroupBox {
      title: "MMComponents.MMSwitch"
      width: parent.width - parent.leftPadding - parent.rightPadding
      background: Rectangle {
        color: "lightGray"
        border.color: "gray"
      }
      label: Label {
        color: "black"
        text: parent.title
        padding: 5
      }

      Flow {
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
        MMComponents.MMSwitch {
          text: "disabled"
          enabled: false
          checked: true
        }
      }
    }
  }
}
