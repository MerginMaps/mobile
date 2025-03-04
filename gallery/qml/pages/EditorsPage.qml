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

import "../../app/qml/inputs"
import "../../app/qml/form/editors" as MMFormEditors
import "../../app/qml/components"
import "../components" as GalleryComponents

ScrollView {
  Column {
    padding: 20
    spacing: 20

    GroupBox {
      title: "Items based on MMBaseInput"
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
        width: ApplicationWindow.window ? ApplicationWindow.window.width - 60 : 0

        MMCheckBox {
          id: checkbox
          text: checked ? "enabled: yes" : "enabled: no"
          checked: true
        }

        MMCheckBox {
          id: checkboxRemember
          text: checked ? "remeber: yes" : "remember: no"
          checked: false
        }

        MMCheckBox {
          id: checkboxTitle
          text: checked ? "show title: yes" : "show title: no"
          checked: true
        }

        MMCheckBox {
          id: checkboxWarning
          text: checked ? "show warning: yes" : "show warning: no"
          checked: false
        }

        MMCheckBox {
          id: checkboxError
          text: checked ? "show error: yes" : "show error: no"
          checked: false
        }

        MMCheckBox {
          id: checkboxEditable
          text: checked ? "attribute editable: yes" : "attribute editable: no"
          checked: true
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: relationEditor.height
          fieldTitle: "MMFormRelationEditor"

          MMFormEditors.MMFormRelationEditor {
            id: relationEditor
            width: parent.width

            onCreateLinkedFeature: function(parentFeature, relation) {
              console.log("Add feature: " + parentFeature + " " + relation)
            }

            onOpenLinkedFeature: function(linkedFeature) {
              console.log("Feature: " + linkedFeature)
            }
          }
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: relationReferenceEditor.height
          fieldTitle: "MMFormRelationReferenceEditor"

          MMFormEditors.MMFormRelationReferenceEditor {
            id: relationReferenceEditor
            width: parent.width
            _fieldValue: "feature1"

            onOpenLinkedFeature: function(linkedFeature) {
              console.log("Feature: " + linkedFeature)
            }
          }
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: galleryEditor.height
          fieldTitle: "MMFormGalleryEditor"

          MMFormEditors.MMFormGalleryEditor {
            id: galleryEditor
            width: parent.width

            onCreateLinkedFeature: function(parentFeature, relation) {
              console.log("Add feature: " + parentFeature + " " + relation)
            }

            onOpenLinkedFeature: function(linkedFeature) {
              console.log("Feature: " + linkedFeature)
            }
          }
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: sliderEditor.height

          fieldValue: "100"
          fieldConfig: ({Min:-100, Max: 100, Suffix: "s", Precision: 1})
          fieldTitle: "MMFormSliderEditor"

          MMFormEditors.MMFormSliderEditor {
            id: sliderEditor

            width: parent.width

            onEditorValueChanged: function(newValue, isNull) { console.log("new value: " + newValue) }
          }
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: numberEditor.height

          fieldValue: "2"
          fieldConfig: ({Min: 1.0, Max: 3.0, Precision: 1, Suffix: "s.", Step: 0.1})
          fieldTitle: "MMFormNumberEditor"

          MMFormEditors.MMFormNumberEditor {
            id: numberEditor

            width: parent.width

            onEditorValueChanged: function(newValue, isNull) { console.log("new value: " + newValue) }
          }
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: scannerEditor.height

          fieldTitle: "MMFormScannerEditor"

          MMFormEditors.MMFormScannerEditor {
            id: scannerEditor

            placeholderText: "QR code"
            width: parent.width

            onEditorValueChanged: function(newValue, isNull) { console.log("new value: " + newValue) }
          }
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: photoEditor.height

          fieldConfig: ({RelativeStorage: 0})
          fieldTitle: "MMFormPhotoEditor"

          MMFormEditors.MMFormPhotoEditor {
            id: photoEditor
            width: parent.width

            onTrashClicked: console.log("Move to trash")
            onChooseFromGalleryClicked: console.log("Choose from gallery")
            onCapturePhotoClicked: console.log("Capture photo")
          }
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: textMultilineEditor.height

          fieldTitle: "MMFormTextMultilineEditor"
          fieldValue: "See something powerfull <a href='https://merginmaps.com/docs/' style='color: green;'>Mergin Maps documentation</a>, for more information continue <a href='https://merginmaps.com/docs/'>here</a>."

          MMFormEditors.MMFormTextMultilineEditor {
            id: textMultilineEditor
            width: parent.width
          }
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: switchEditor.height

          fieldConfig: ({ CheckedState: "checked", UncheckedState: "unchecked"})
          fieldTitle: "MMFormSwitchEditor"

          MMFormEditors.MMFormSwitchEditor {
            id: switchEditor
            width: parent.width
          }
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: dateTimeCalendar.height
          fieldTitle: "MMFormCalendarEditor (Date & Time)"
          fieldValue: "2013-09-17 10:22"
          fieldConfig: ({
            field_format: "yyyy-MM-dd hh:mm",
            display_format: "yyyy-MM-dd hh:mm"
          })
          field: ""

          MMFormEditors.MMFormCalendarEditor {
            id: dateTimeCalendar

            text: "yyyy-MM-dd hh:mm"
            width: parent.width

            fieldIsDate: false
            includesTime: true
            includesDate: true

            onEditorValueChanged: function(newValue, isNull) { console.log("new value: " + newValue) }
          }
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: dateCalendar.height
          fieldTitle: "MMFormCalendarEditor (Date)"
          fieldValue: "2013-09-17"
          fieldConfig: ({
            field_format: "yyyy-MM-dd",
            display_format: "yyyy-MM-dd"
          })
          field: ""

          MMFormEditors.MMFormCalendarEditor {
            id: dateCalendar

            text: "yyyy-MM-dd"
            enabled: checkbox.checked
            width: parent.width
            warningMsg: text.length > 0 ? "" : "Press button to open Calendar"

            fieldIsDate: false
            includesTime: false
            includesDate: true

            onEditorValueChanged: function(newValue, isNull) { console.log("new value: " + newValue) }
          }
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: timeCalendar.height
          fieldTitle: "MMFormCalendarEditor (Time)"
          fieldValue: "2013-09-17 11:22"
          fieldConfig: ({
            field_format: "yyyy-MM-dd hh:mm",
            display_format: "hh:mm"
          })
          field: ""

          MMFormEditors.MMFormCalendarEditor {
            id: timeCalendar

            text: "hh:mm"
            enabled: checkbox.checked
            width: parent.width
            warningMsg: text.length > 0 ? "" : "Press button to open Calendar"

            fieldIsDate: false
            includesTime: true
            includesDate: false
            showSeconds: true

            onEditorValueChanged: function(newValue, isNull) { console.log("new value: " + newValue) }
          }
        }

        Label {
          text: "MMFormSpacer - HLine"
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: spacer2.height

          fieldConfig: ({IsHLine: true})
          fieldTitle: "title not shown for spacer"

          MMFormEditors.MMFormSpacer {
            id: spacer2
            width: parent.width
          }
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: richTextViewer.height

          fieldValue: "<h3>Lorem ipsum</h3> dolor sit amet,        consectetur adipiscing elit,\n" +
                      " sed do eiusmod tempor \n\nincididunt ut labore et dolore magna aliqua.<br><br><br>" +
                      "See something powerfull <a href='https://merginmaps.com/docs/' style='color: green;'>Mergin Maps documentation</a>, for more information continue <a href='https://merginmaps.com/docs/'>here</a>."

          fieldConfig: ({UseHtml: false})
          fieldTitle: "MMFormRichTextViewer - Text"

          MMFormEditors.MMFormRichTextViewer {
            id: richTextViewer
            width: parent.width
          }
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: richTextViewerHtml.height

          fieldValue: "<h3>Lorem ipsum</h3> dolor sit amet,        consectetur adipiscing elit,\n" +
                      " sed do eiusmod tempor \n\nincididunt ut labore et dolore magna aliqua.<br><br><br>" +
                      "See something powerfull <a href='https://merginmaps.com/docs/' style='color: green;'>Mergin Maps documentation</a>, for more information continue <a href='https://merginmaps.com/docs/'>here</a>."

          fieldConfig: ({UseHtml: true})
          fieldTitle: "MMFormRichTextViewer - HTML"

          MMFormEditors.MMFormRichTextViewer {
            id: richTextViewerHtml
            width: parent.width
          }
        }

        GalleryComponents.EditorItem {
          width: parent.width
          height: valueMapEditor.height

          fieldValue: "0"
          fieldConfig: ({ map: [ {"First": 0}, {"Second": 1} ] })
          fieldTitle: "MMFormValueMapEditor - Value map"

          MMFormEditors.MMFormValueMapEditor {
            id: valueMapEditor
            width: parent.width

            onEditorValueChanged: function(newValue, isNull) {
              console.log("new value: " + newValue)
              parent.fieldValue = newValue
            }
          }
        }
      }
    }
  }
}
