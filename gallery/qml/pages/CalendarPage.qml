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
import "../components" as GalleryComponents

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

        GalleryComponents.EditorItem {
          width: parent.width
          height: 100
          fieldTitle: "MMFormGalleryEditor"
          fieldValue: "2013-09-17 11:22"
          fieldConfig: ({
            field_format: "yyyy-MM-dd hh:mm",
            display_format: "hh:mm"
          })
          field: ""

          MMFormCalendarEditor {
            title: "Date & Time"
            enabled: checkbox.checked
            width: parent.width
            warningMsg: text.length > 0 ? "" : "Press button to open Calendar"

            fieldIsDate: false
            includesTime: true
            includesDate: true
          }
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: 100
          fieldTitle: "MMFormGalleryEditor"
          fieldValue: "2013-09-17 11:22"
          fieldConfig: ({
            field_format: "yyyy-MM-dd hh:mm",
            display_format: "hh:mm"
          })
          field: ""

          MMFormCalendarEditor {
            title: "Date"
            enabled: checkbox.checked
            width: parent.width
            warningMsg: text.length > 0 ? "" : "Press button to open Calendar"

            fieldIsDate: false
            includesTime: false
            includesDate: true
          }
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: 100
          fieldTitle: "MMFormGalleryEditor"
          fieldValue: "2013-09-17 11:22"
          fieldConfig: ({
            field_format: "yyyy-MM-dd hh:mm",
            display_format: "hh:mm"
          })
          field: ""

          MMFormCalendarEditor {
            title: "Time"
            enabled: checkbox.checked
            width: parent.width
            warningMsg: text.length > 0 ? "" : "Press button to open Calendar"

            fieldIsDate: false
            includesTime: true
            includesDate: false
            showSeconds: true
          }
        }
      }
    }
  }
}
