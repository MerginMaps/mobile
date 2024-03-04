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
import QtQuick.Dialogs
import QtQuick.Layouts

import "../components"


MMDrawerDialog {
  id: root

  property int maxProjectNumber: -1
  required property string plan
  required property bool apiSupportsSubscription

  picture: __style.reachedDataLimitImage
  bigTitle: qsTr("You have reached project number limit")
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
        text: qsTr("Maximum of projects")
        font: __style.p5
        color: __style.nightColor
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
      }
      Text {
        id: dataToSyncText

        width: parent.width
        text: root.maxProjectNumber
        font: __style.t3
        color: __style.nightColor
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
      }
    }

    Rectangle { width: parent.width; height: 1; color: __style.greyColor }

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
