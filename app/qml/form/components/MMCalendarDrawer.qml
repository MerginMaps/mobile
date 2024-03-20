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

import "../../components" as MMComponents
import "./calendar"

MMComponents.MMDrawer {
  id: root

  property alias title: title.text
  property alias dateTime: dateTimePicker.dateToSelect
  property alias hasDatePicker: dateTimePicker.hasDatePicker
  property alias hasTimePicker: dateTimePicker.hasTimePicker
  property bool showSeconds: false

  signal primaryButtonClicked

  height: mainColumn.height
  dim: true

  drawerContent: Rectangle {
    id: roundedRect

    anchors.fill: parent
    color: __style.polarColor

    Column {
      id: mainColumn

      width: parent.width
      spacing: __style.spacing20
      topPadding: __style.pageMargins
      leftPadding: __style.pageMargins
      rightPadding: __style.pageMargins
      bottomPadding: __style.pageMargins

      Text {
        id: title

        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - mainColumn.leftPadding - mainColumn.rightPadding - 2 * parent.spacing
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        font: __style.t2
        color: __style.forestColor
      }

      MMDateTimePicker {
        id: dateTimePicker

        width: parent.width
        showSeconds: root.showSeconds
      }

      MMComponents.MMButton {
        id: primaryButton

        width: parent.width - 2 * parent.spacing
        visible: text.length > 0
        text: qsTr("Confirm")

        onClicked: {
          dateTimePicker.visible = false
          primaryButtonClicked()
          close()
        }
      }
    }
  }
}
