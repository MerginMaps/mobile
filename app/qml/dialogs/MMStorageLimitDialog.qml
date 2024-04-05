/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../components" as MMComponents


MMComponents.MMDrawerDialog {
  id: root

  property string dataToSync
  required property string dataUsing
  required property real usedData // [0 - 1]
  required property bool apiSupportsSubscription

  signal manageAccountClicked()

  title: qsTr("You have reached a data limit")
  imageSource: __style.reachedDataLimitImage

  primaryButton.text: apiSupportsSubscription ? qsTr("Manage account") : ""

  additionalContent: Column {
    width: parent.width
    spacing: 10 * __dp
    topPadding: 20 * __dp

    Item {
      width: parent.width
      height: dataToSyncText.height

      MMComponents.MMText {
        width: parent.width
        text: qsTr("Data to sync")
        font: __style.p5
        color: __style.nightColor
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
      }

      MMComponents.MMText {
        id: dataToSyncText

        width: parent.width
        text: root.dataToSync
        font: __style.t3
        color: __style.nightColor
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
      }
    }

    MMComponents.MMLine { width: parent.width }

    Item {
      width: parent.width
      height: dataUsingText.height

      MMComponents.MMText {
        width: parent.width
        text: qsTr("Using")
        font: __style.p5
        color: __style.nightColor
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
      }

      MMComponents.MMText {
        id: dataUsingText

        width: parent.width
        text: root.dataUsing
        font: __style.t3
        color: __style.nightColor
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
      }
    }

    MMComponents.MMProgressBar {
      position: root.usedData
      progressColor: position > 0.9 ? __style.grapeColor : __style.grassColor
      color: position > 0.9 ? __style.negativeColor : __style.lightGreenColor
    }
  }

  onPrimaryButtonClicked: {
    root.manageAccountClicked()
    close()
  }
}
