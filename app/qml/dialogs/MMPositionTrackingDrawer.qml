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
import QtQuick.Layouts

import "./components" as MMComponents

Drawer {
  id: root

  property bool trackingActive: false
  property string distanceTraveled
  property string trackingStartedAt

  signal trackingBtnClicked()

  edge: Qt.BottomEdge
  closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

  leftPadding: InputStyle.panelMargin
  rightPadding: InputStyle.panelMargin
  bottomPadding: InputStyle.smallGap
  topPadding: InputStyle.panelMarginV2

  ColumnLayout {
    anchors.fill: parent

    spacing: 0

    MMComponents.PanelHeaderV2 {
      Layout.fillWidth: true
      headerTitle: qsTr("Position tracking")
      haveBackButton: false
      onBackClicked: root.close()
    }

    // spacer
    Item {
      Layout.preferredHeight: InputStyle.verticalSpaceM
    }

    Text {
      text: qsTr("Mergin Maps can track your position on this project.")
      horizontalAlignment: Text.AlignHCenter
      Layout.fillWidth: true
      wrapMode: Text.WordWrap
    }

    // spacer
    Item {
      Layout.preferredHeight: InputStyle.verticalSpaceM
    }

    // data

    RowLayout {
      Layout.fillWidth: true

      Label {
        text: "Tracked distance:"
        elide: Text.ElideRight
        Layout.preferredWidth: contentWidth
      }

      Label {
        font.bold: true
        text: root.distanceTraveled ?? "---"
        Layout.fillWidth: true
        horizontalAlignment: Text.AlignRight
      }
    }

    RowLayout {
      Layout.fillWidth: true

      Label {
        text: "Started at:"
        elide: Text.ElideRight
        Layout.preferredWidth: contentWidth
      }

      Label {
        font.bold: true
        text: root.trackingStartedAt ?? "---"
        Layout.fillWidth: true
        horizontalAlignment: Text.AlignRight
      }
    }

    // spacer
    Item {
      Layout.preferredHeight: InputStyle.verticalSpaceM
    }

    Button {
      id: trackingButton

      text: root.trackingActive ? qsTr("Stop tracking") : qsTr("Start tracking")

      Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

      background: Rectangle {
        color: trackingButton.down ? InputStyle.actionColorDown : InputStyle.actionColor
        radius: InputStyle.cornerRadius
      }

      contentItem: Text {
        color: "white"
        font.bold: true
        text: trackingButton.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
      }

      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.mediumBtnHeight

      onClicked: {
        root.trackingBtnClicked()
      }
    }
  }
}
