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
import "."
import ".."

Item {
  id: control

  property Component comp: component
  required property string dataToSync
  required property string dataUsing
  required property string plan
  required property real usedData // [0 = 1]

  width: parent.width

  Component {
    id: component

    Column {
      width: control.width - 40 * __dp
      spacing: 10 * __dp
      topPadding: 20 * __dp

      Item {
        width: parent.width
        height: dataToSyncText.height

        Text {
          width: parent.width
          text: qsTr("Data to sync")
          font: StyleV2.p5
          color: StyleV2.nightColor
          horizontalAlignment: Text.AlignLeft
          verticalAlignment: Text.AlignVCenter
        }
        Text {
          id: dataToSyncText

          width: parent.width
          text: control.dataToSync
          font: StyleV2.t3
          color: StyleV2.nightColor
          horizontalAlignment: Text.AlignRight
          verticalAlignment: Text.AlignVCenter
        }
      }

      Rectangle { width: parent.width; height: 1; color: StyleV2.grayColor }

      Item {
        width: parent.width
        height: dataUsingText.height

        Text {
          width: parent.width
          text: qsTr("Using")
          font: StyleV2.p5
          color: StyleV2.nightColor
          horizontalAlignment: Text.AlignLeft
          verticalAlignment: Text.AlignVCenter
        }
        Text {
          id: dataUsingText

          width: parent.width
          text: control.dataUsing
          font: StyleV2.t3
          color: StyleV2.nightColor
          horizontalAlignment: Text.AlignRight
          verticalAlignment: Text.AlignVCenter
        }
      }

      MMProgressBar {
        position: control.usedData
      }

      Rectangle { width: parent.width; height: 1; color: StyleV2.grayColor }

      Item {
        width: parent.width
        height: planText.height

        Text {
          width: parent.width
          text: qsTr("Plan")
          font: StyleV2.p5
          color: StyleV2.nightColor
          horizontalAlignment: Text.AlignLeft
          verticalAlignment: Text.AlignVCenter
        }
        Text {
          id: planText

          width: parent.width
          text: control.plan
          font: StyleV2.t3
          color: StyleV2.nightColor
          horizontalAlignment: Text.AlignRight
          verticalAlignment: Text.AlignVCenter
        }
      }
    }
  }
}
