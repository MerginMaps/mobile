/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "../components"
import "../inputs"

Page {
  id: root

  signal continueClicked

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  MMHeader {
    id: header

    x: mainColumn.padding
    y: mainColumn.padding
    width: parent.width - 2 * mainColumn.padding
    backVisible: false
    step: 1

    onBackClicked: root.backClicked()
  }

  ScrollView {
    width: parent.width
    height: parent.height - header.height - bottomColumn.height - 40 * __dp
    anchors.top: header.bottom
    anchors.topMargin: 20 * __dp

    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
    ScrollBar.vertical.policy: ScrollBar.AlwaysOff

    Column {
      id: mainColumn

      width: root.width
      spacing: 20 * __dp
      padding: 20 * __dp

      Text {
        width: parent.width - 2 * mainColumn.padding
        text: qsTr("Create a workspace")
        font: __style.h3
        color: __style.forestColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        lineHeight: 1.2
      }

      Text {
        width: parent.width - mainColumn.leftPadding - mainColumn.rightPadding
        text: qsTr("Workspace is a place to store your projects. Colleagues can be invited to your workspace to collaborate on projects.")
        font: __style.p5
        color: __style.nightColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        lineHeight: 1.5
      }

      Item { width: 1; height: 1 }

      MMInputEditor {
        width: parent.width - 2 * mainColumn.padding
        title: qsTr("Workspace name")
        placeholderText: qsTr("Your Workspace")
      }
    }
  }

  Column {
    id: bottomColumn

    width: root.width
    spacing: 20 * __dp
    padding: 20 * __dp
    anchors.bottom: parent.bottom

    MMTextBubble {
      width: root.width - 2 * mainColumn.padding
      title: "Tip from Mergin Maps"
      description: "A good candidate for a workspace name is the name of your team or organisation"
    }

    MMButton {
      width: parent.width - 2 * mainColumn.padding
      text: qsTr("Create workspace")

      onClicked: root.continueClicked()
    }
  }
}
