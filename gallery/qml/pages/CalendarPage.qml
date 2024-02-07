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

import "../../app/qml/form/editors"
import "../../app/qml/components"

ScrollView {
  Column {
    padding: 20
    spacing: 20

    GroupBox {
      title: "MMCalendarEditor"
      background: Rectangle {
        color: "lightGray"
        border.color: "gray"
      }
      label: Label {
        color: "black"
        text: parent.title
        padding: 5
      }

      Column {
        spacing: 10
        width: 300

        MMCheckBox {
          id: checkbox
          text: checked ? "enabled" : "disabled"
          checked: true
        }

        MMCalendarFormEditor {
          title: "Date & Time"
          placeholderText: "YYYY/MM/DD HH:MM"
          enabled: checkbox.checked
          width: parent.width
          warningMsg: text.length > 0 ? "" : "Press button to open Calendar"
          config: {'field_format':'?', 'display_format':'yyyy'}

          dateTime: new Date()
          fieldIsDate: false
          includesTime: true
          includesDate: true

          onSelected: function(newDateTime) { dateTime = newDateTime; text = Qt.formatDateTime(newDateTime, "yyyy/MM/dd hh:mm") }
        }

        MMCalendarFormEditor {
          title: "Date"
          placeholderText: "YYYY/MM/DD"
          enabled: checkbox.checked
          width: parent.width
          warningMsg: text.length > 0 ? "" : "Press button to open Calendar"
          config: {'field_format':'?', 'display_format':'yyyy'}

          dateTime: new Date()
          fieldIsDate: false
          includesTime: false
          includesDate: true

          onSelected: function(newDateTime) { dateTime = newDateTime; text = Qt.formatDateTime(newDateTime, "yyyy/MM/dd") }
        }

        MMCalendarFormEditor {
          title: "Time"
          placeholderText: "HH:MM:SS"
          enabled: checkbox.checked
          width: parent.width
          warningMsg: text.length > 0 ? "" : "Press button to open Calendar"
          config: {'field_format':'?', 'display_format':'yyyy'}

          dateTime: new Date()
          fieldIsDate: false
          includesTime: true
          includesDate: false
          showSeconds: true

          onSelected: function(newDateTime) { dateTime = newDateTime; text = Qt.formatDateTime(newDateTime, "hh:mm:ss") }
        }
      }
    }
  }
}
