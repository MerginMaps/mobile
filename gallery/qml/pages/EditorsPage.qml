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

        Item {
          width: parent.width
          height: relationEditor.height

          property var fieldValue: ""
          property var fieldConfig:  ({})
          property bool fieldShouldShowTitle: checkboxTitle.checked
          property bool fieldIsReadOnly: !checkbox.checked
          property string fieldTitle: "MMFormRelationEditor"
          property string fieldErrorMessage: checkboxError.checked ? "error" : ""
          property string fieldWarningMessage: checkboxWarning.checked ? "warning" : ""
          property bool fieldRememberValueSupported: checkboxRemember.checked
          property bool fieldRememberValueState: false

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

        Item {
          width: parent.width
          height: galleryEditor.height

          property var fieldValue: ""
          property var fieldConfig: ({})
          property bool fieldShouldShowTitle: checkboxTitle.checked
          property bool fieldIsReadOnly: !checkbox.checked
          property string fieldTitle: "MMFormGalleryEditor"
          property string fieldErrorMessage: checkboxError.checked ? "error" : ""
          property string fieldWarningMessage: checkboxWarning.checked ? "warning" : ""
          property bool fieldRememberValueSupported: checkboxRemember.checked
          property bool fieldRememberValueState: false

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

        Item {
          width: parent.width
          height: sliderEditor.height

          property var fieldValue: -100
          property var fieldConfig: {["Min",-100],["Max", 100], ["Suffix", "s"], ["Precision", 1]}
          property bool fieldShouldShowTitle: checkboxTitle.checked
          property bool fieldIsReadOnly: !checkbox.checked
          property string fieldTitle: "MMFormSliderEditor"
          property string fieldErrorMessage: checkboxError.checked ? "error" : ""
          property string fieldWarningMessage: checkboxWarning.checked ? "warning" : ""
          property bool fieldRememberValueSupported: checkboxRemember.checked
          property bool fieldRememberValueState: false

          MMFormSliderEditor {
            id: sliderEditor
            width: parent.width
            onEditorValueChanged: function(newValue, isNull ) { parent.fieldValue = newValue }
          }
        }

        Item {
          width: parent.width
          height: numberEditor.height

          property var fieldValue: 2
          property var fieldConfig: {["Min",1.0], ["Max", 3.0], ["Precition", 1], ["Suffix", "s."], ["Step", 0.1]}
          property bool fieldShouldShowTitle: checkboxTitle.checked
          property bool fieldIsReadOnly: !checkbox.checked
          property string fieldTitle: "MMFormNumberEditor"
          property string fieldErrorMessage: checkboxError.checked ? "error" : ""
          property string fieldWarningMessage: checkboxWarning.checked ? "warning" : ""
          property bool fieldRememberValueSupported: checkboxRemember.checked
          property bool fieldRememberValueState: false
          property bool fieldValueIsNull: false

          MMFormNumberEditor {
            id: numberEditor
            width: parent.width
            onEditorValueChanged: function(newValue, isNull) { parent.fieldValue = newValue }
          }
        }

        Item {
          width: parent.width
          height: scannerEditor.height

          property var fieldValue: ""
          property var fieldConfig: ({})
          property bool fieldShouldShowTitle: checkboxTitle.checked
          property bool fieldIsReadOnly: !checkbox.checked
          property string fieldTitle: "MMFormScannerEditor"
          property string fieldErrorMessage: checkboxError.checked ? "error" : ""
          property string fieldWarningMessage: checkboxWarning.checked ? "warning" : ""
          property bool fieldRememberValueSupported: checkboxRemember.checked
          property bool fieldRememberValueState: false
          property bool fieldValueIsNull: false

          MMFormScannerEditor {
            id: scannerEditor
            placeholderText: "QR code"
            width: parent.width
            onEditorValueChanged: function(newValue, isNull) { parent.fieldValue = newValue }
          }
        }

        Item {
          width: parent.width
          height: photoEditor.height
          property var fieldValue: ""
          property var fieldConfig: {["RelativeStorage", ""]}
          property bool fieldShouldShowTitle: checkboxTitle.checked
          property bool fieldIsReadOnly: !checkbox.checked
          property string fieldTitle: "MMFormPhotoEditor"
          property string fieldErrorMessage: checkboxError.checked ? "error" : ""
          property string fieldWarningMessage: checkboxWarning.checked ? "warning" : ""
          property bool fieldRememberValueSupported: checkboxRemember.checked
          property bool fieldRememberValueState: false
          property bool fieldValueIsNull: false

          MMFormPhotoEditor {
            id: photoEditor
            width: parent.width

            onTrashClicked: console.log("Move to trash")
            onContentClicked: console.log("Open photo")
          }
        }

        Item {
          width: parent.width
          height: textMultilineEditor.height

          property var fieldValue: ""
          property var fieldConfig: ({})
          property bool fieldShouldShowTitle: checkboxTitle.checked
          property bool fieldIsReadOnly: !checkbox.checked
          property string fieldTitle: "MMFormTextMultilineEditor"
          property string fieldErrorMessage: checkboxError.checked ? "error" : ""
          property string fieldWarningMessage: checkboxWarning.checked ? "warning" : ""
          property bool fieldRememberValueSupported: checkboxRemember.checked
          property bool fieldRememberValueState: false
          property bool fieldValueIsNull: false

          MMFormTextMultilineEditor {
            id: textMultilineEditor
            placeholderText: "Place for multi row text"
            width: parent.width
          }
        }

        Item {
          width: parent.width
          height: switchEditor.height

          property var fieldValue: ""
          property var fieldConfig: {["CheckedState", "checked"], ["UncheckedState", "unchecked"]}
          property bool fieldShouldShowTitle: checkboxTitle.checked
          property bool fieldIsReadOnly: !checkbox.checked
          property string fieldTitle: "MMFormSwitchEditor"
          property string fieldErrorMessage: checkboxError.checked ? "error" : ""
          property string fieldWarningMessage: checkboxWarning.checked ? "warning" : ""
          property bool fieldRememberValueSupported: checkboxRemember.checked
          property bool fieldRememberValueState: false
          property bool fieldValueIsNull: false

          MMFormSwitchEditor {
            id: switchEditor
            width: parent.width
          }
        }
      }
    }
  }
}
