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
import "../../app/qml/form/editors"
import "../../app/qml/components"
import "../"

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
        width: ApplicationWindow.window ? ApplicationWindow.window.width - 40 : 0

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

        EditorItem {
          width: parent.width
          height: relationEditor.height
          fieldTitle: "MMFormRelationEditor"

          MMFormRelationEditor {
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

        EditorItem {
          width: parent.width
          height: galleryEditor.height
          fieldTitle: "MMFormGalleryEditor"

          MMFormGalleryEditor {
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

        EditorItem {
          width: parent.width
          height: sliderEditor.height

          fieldValue: "100"
          fieldConfig: ({Min:-100, Max: 100, Suffix: "s", Precision: 1})
          fieldTitle: "MMFormSliderEditor"

          MMFormSliderEditor {
            id: sliderEditor
            width: parent.width
            onEditorValueChanged: function(newValue, isNull ) { parent.fieldValue = newValue }
          }
        }

        EditorItem {
          width: parent.width
          height: numberEditor.height

          fieldValue: "2"
          fieldConfig: ({Min: 1.0, Max: 3.0, Precition: 1, Suffix: "s.", Step: 0.1})
          fieldTitle: "MMFormNumberEditor"

          MMFormNumberEditor {
            id: numberEditor
            width: parent.width
            onEditorValueChanged: function(newValue, isNull) { parent.fieldValue = newValue }
          }
        }

        EditorItem {
          width: parent.width
          height: scannerEditor.height

          fieldTitle: "MMFormScannerEditor"

          MMFormScannerEditor {
            id: scannerEditor
            placeholderText: "QR code"
            width: parent.width
            onEditorValueChanged: function(newValue, isNull) { parent.fieldValue = newValue }
          }
        }

        EditorItem {
          width: parent.width
          height: photoEditor.height

          fieldConfig: ({RelativeStorage: ""})
          fieldTitle: "MMFormPhotoEditor"

          MMFormPhotoEditor {
            id: photoEditor
            width: parent.width

            onTrashClicked: console.log("Move to trash")
            onContentClicked: console.log("Open photo")
          }
        }

        EditorItem {
          width: parent.width
          height: textMultilineEditor.height

          fieldTitle: "MMFormTextMultilineEditor"

          MMFormTextMultilineEditor {
            id: textMultilineEditor
            placeholderText: "Place for multi row text"
            width: parent.width
          }
        }

        EditorItem {
          width: parent.width
          height: switchEditor.height

          fieldConfig: ({ CheckedState: "checked", UncheckedState: "unchecked"})
          fieldTitle: "MMFormSwitchEditor"

          MMFormSwitchEditor {
            id: switchEditor
            width: parent.width
          }
        }

        Label {
          text: "MMFormSpacer - HLine"
        }

        EditorItem {
          width: parent.width
          height: spacer2.height

          fieldConfig: ({IsHLine: true})
          fieldTitle: "title not shown for spacer"

          MMFormSpacer {
            id: spacer2
            width: parent.width
          }
        }


        Label {
          text: "MMFormRichTextViewer - Text"
        }

        EditorItem {
          width: parent.width
          height: richTextViewer.height

          fieldValue: "Lorem ipsum dolor sit amet, consectetur adipiscing elit," +
                      " sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n\n\n" +
                      " Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris" +
                      " nisi ut aliquip ex ea commodo consequat."

          fieldConfig: ({UseHtml: false})
          fieldTitle: ""

          MMFormRichTextViewer {
            id: richTextViewer
            width: parent.width
          }
        }
      }
    }
  }
}
