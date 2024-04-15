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

  property string title
  property alias dateTime: dateTimePicker.dateToSelect
  property alias hasDatePicker: dateTimePicker.hasDatePicker
  property alias hasTimePicker: dateTimePicker.hasTimePicker
  property bool showSeconds: false

  signal primaryButtonClicked

  dim: true

  drawerHeader.title: root.title

  drawerContent: Item {
      width: parent.width
      height: scrollView.height

      MMComponents.MMScrollView {
      id: scrollView

      width: parent.width
      height: root.maxHeightHit ? root.drawerContentAvailableHeight : contentHeight

      Column {
        width: parent.width
        spacing: __style.spacing20

        MMDateTimePicker {
          id: dateTimePicker

          width: parent.width
          showSeconds: root.showSeconds
        }

        MMComponents.MMButton {
          id: primaryButton

          width: parent.width
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
}
