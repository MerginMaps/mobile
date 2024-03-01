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

  signal createWorkspaceClicked(string name)
  signal backClicked

  property bool showProgress: true
  property bool backVisible: false

  readonly property real hPadding: width < __style.maxPageWidth
                                   ? 20 * __dp
                                   : (20 + (width - __style.maxPageWidth) / 2) * __dp

  // show error message under the respective field
  function showErrorMessage( msg ) {
    workspaceName.errorMsg = msg
  }

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  MMPageHeader {
    id: header

    backVisible: root.backVisible

    onBackClicked: root.backClicked()

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
      position: 1/3
      visible: root.showProgress
    }
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
      leftPadding: root.hPadding
      rightPadding: root.hPadding
      topPadding: 20 * __dp
      bottomPadding: 20 * __dp

      Text {
        width: parent.width - 2 * root.hPadding
        text: qsTr("Create a workspace")
        font: __style.h3
        color: __style.forestColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        lineHeight: 1.2
      }

      Text {
        width: parent.width - 2 * root.hPadding
        text: qsTr("Workspace is a place to store your projects. Colleagues can be invited to your workspace to collaborate on projects.")
        font: __style.p5
        color: __style.nightColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        lineHeight: 1.5
      }

      Item { width: 1; height: 1 }

      MMTextInput {
        id: workspaceName
        width: parent.width - 2 * root.hPadding
        title: qsTr("Workspace name")
        placeholderText: qsTr("Your Workspace")
      }
    }
  }

  Column {
    id: bottomColumn

    width: root.width
    spacing: 20 * __dp
    leftPadding: root.hPadding
    rightPadding: root.hPadding
    topPadding: 20 * __dp
    bottomPadding: 20 * __dp
    anchors.bottom: parent.bottom

    MMTextBubble {
      width: root.width - 2 * root.hPadding
      title: qsTr("Tip from Mergin Maps")
      description: qsTr("A good candidate for a workspace name is the name of your team or organisation")
    }

    MMButton {
      width: parent.width - 2 * root.hPadding
      text: qsTr("Create workspace")

      onClicked: {
        if (workspaceName.text.length > 0 ) {
          root.createWorkspaceClicked(workspaceName.text)
        } else {
          showErrorMessage(qsTr("Workspace name cannot be empty."))
        }
      }
    }
  }
}
