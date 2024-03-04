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

import "../components"


MMDrawerDialog {
  id: root

  property string dataToSync
  required property string dataUsing
  required property string plan
  required property real usedData // [0 = 1]
  required property bool apiSupportsSubscription

  picture: __style.reachedDataLimitImage
  bigTitle: qsTr("You have reached a data limit")
  primaryButton: apiSupportsSubscription ? qsTr("Manage account") : ""

  signal manageAccountClicked()

  specialComponent: Column {
    width: root.width - 40 * __dp
    spacing: 10 * __dp
    topPadding: 20 * __dp

    Item {
      width: parent.width
      height: dataToSyncText.height

      Text {
        width: parent.width
        text: qsTr("Data to sync")
        font: __style.p5
        color: __style.nightColor
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
      }
      Text {
        id: dataToSyncText

        width: parent.width
        text: root.dataToSync
        font: __style.t3
        color: __style.nightColor
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
      }
    }

    MMLine { width: parent.width }

    Item {
      width: parent.width
      height: dataUsingText.height

      Text {
        width: parent.width
        text: qsTr("Using")
        font: __style.p5
        color: __style.nightColor
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
      }
      Text {
        id: dataUsingText

        width: parent.width
        text: root.dataUsing
        font: __style.t3
        color: __style.nightColor
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
      }
    }

    MMProgressBar {
      position: root.usedData
    }

    MMLine { width: parent.width }

    Item {
      width: parent.width
      height: planText.height

      Text {
        width: parent.width
        text: qsTr("Plan")
        font: __style.p5
        color: __style.nightColor
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
      }
      Text {
        id: planText

        width: parent.width
        text: root.plan
        font: __style.t3
        color: __style.nightColor
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
      }
    }
  }

  onPrimaryButtonClicked: {
    root.manageAccountClicked()
    close()
  }
}
