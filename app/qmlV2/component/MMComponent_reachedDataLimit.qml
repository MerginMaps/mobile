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
import "../Style.js" as Style
import "."

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
      width: control.width - 2 * Style.commonSpacing
      spacing: Style.commonSpacing / 2
      topPadding: Style.commonSpacing

      Item {
        width: parent.width
        height: dataToSyncText.height

        Text {
          width: parent.width
          text: qsTr("Data to sync")
          font: Qt.font(Style.p5)
          color: Style.night
          horizontalAlignment: Text.AlignLeft
          verticalAlignment: Text.AlignVCenter
        }
        Text {
          id: dataToSyncText

          width: parent.width
          text: control.dataToSync
          font: Qt.font(Style.t3)
          color: Style.night
          horizontalAlignment: Text.AlignRight
          verticalAlignment: Text.AlignVCenter
        }
      }

      Rectangle { width: parent.width; height: 1; color: Style.gray }

      Item {
        width: parent.width
        height: dataUsingText.height

        Text {
          width: parent.width
          text: qsTr("Using")
          font: Qt.font(Style.p5)
          color: Style.night
          horizontalAlignment: Text.AlignLeft
          verticalAlignment: Text.AlignVCenter
        }
        Text {
          id: dataUsingText

          width: parent.width
          text: control.dataUsing
          font: Qt.font(Style.t3)
          color: Style.night
          horizontalAlignment: Text.AlignRight
          verticalAlignment: Text.AlignVCenter
        }
      }

      MMProgressBar {
        position: control.usedData
      }

      Rectangle { width: parent.width; height: 1; color: Style.gray }

      Item {
        width: parent.width
        height: planText.height

        Text {
          width: parent.width
          text: qsTr("Plan")
          font: Qt.font(Style.p5)
          color: Style.night
          horizontalAlignment: Text.AlignLeft
          verticalAlignment: Text.AlignVCenter
        }
        Text {
          id: planText

          width: parent.width
          text: control.plan
          font: Qt.font(Style.t3)
          color: Style.night
          horizontalAlignment: Text.AlignRight
          verticalAlignment: Text.AlignVCenter
        }
      }
    }
  }
}
