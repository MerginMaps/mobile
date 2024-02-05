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
          text: checked ? "enabled" : "disabled"
          checked: true
        }

        MMRelationsEditor {
          title: "MMTextAreaEditor"
          enabled: checkbox.checked
          width: parent.width

          onCreateLinkedFeature: function(parentFeature, relation) {
            console.log("Add feature: " + parentFeature + " " + relation)
          }

          onOpenLinkedFeature: function(linkedFeature) {
            console.log("Feature: " + linkedFeature)
          }

          featuresModel: ListModel {
            property string searchExpression

            ListElement {
              FeatureId: 1
              FeatureTitle: "Title 1"
              FeaturePair: "Pair 1"
              Description: "Description 1"
              SearchResult: "SearchResult 1"
              Feature: "Feature 1"
            }
            ListElement {
              FeatureId: 2
              FeatureTitle: "Title 2"
              FeaturePair: "Pair 2"
              Description: "Description 2"
              SearchResult: "SearchResult 2"
              Feature: "Feature 2"
            }
            ListElement {
              FeatureId: 3
              FeatureTitle: "Title 3"
              FeaturePair: "Pair 3"
              Description: "Description 3"
              SearchResult: "SearchResult 3"
              Feature: "Feature 3"
            }
            ListElement {
              FeatureId: 4
              FeatureTitle: "Title 4"
              FeaturePair: "Pair 4"
              Description: "Description 4"
              SearchResult: "SearchResult 4"
              Feature: "Feature 4"
            }
            ListElement {
              FeatureId: 5
              FeatureTitle: "Title 5"
              FeaturePair: "Pair 5"
              Description: "Description 5"
              SearchResult: "SearchResult 5"
              Feature: "Feature 5"
            }
            ListElement {
              FeatureId: 6
              FeatureTitle: "Title 6"
              FeaturePair: "Pair 6"
              Description: "Description 6"
              SearchResult: "SearchResult 6"
              Feature: "Feature 6"
            }
            ListElement {
              FeatureId: 7
              FeatureTitle: "Title 7"
              FeaturePair: "Pair 7"
              Description: "Description 7"
              SearchResult: "SearchResult 7"
              Feature: "Feature 7"
            }
          }
        }

        MMSearchEditor {
          title: "MMSearchEditor"
          placeholderText: "Text value"
          onSearchTextChanged: function(text) { console.log("Searched string: " + text) }
        }

        MMDropdownFormEditor {
          title: "MMComboBoxEditor"
          placeholderText: "Select one"
          dropDownTitle: "Select one"
          enabled: checkbox.checked
          width: parent.width
          featuresModel: ListModel {
            ListElement {
              FeatureId: 1
              FeatureTitle: "Title 1"
              Description: "Description 1"
              SearchResult: "SearchResult 1"
              Feature: "Feature 1"
            }
            ListElement {
              FeatureId: 2
              FeatureTitle: "Title 2"
              Description: "Description 2"
              SearchResult: "SearchResult 2"
              Feature: "Feature 2"
            }
            ListElement {
              FeatureId: 3
              FeatureTitle: "Title 3"
              Description: "Description 3"
              SearchResult: "SearchResult 3"
              Feature: "Feature 3"
            }
          }
          onFeatureClicked: function(selectedFeatures) { text = selectedFeatures }
        }

        MMDropdownFormEditor {
          title: "MMComboBoxEditor Multi select"
          placeholderText: "Select multiple"
          dropDownTitle: "Multi select"
          enabled: checkbox.checked
          width: parent.width
          multiSelect: true
          preselectedFeatures: []

          featuresModel: ListModel {
            property string searchExpression

            ListElement {
              FeatureId: 1
              FeatureTitle: "Title 1"
              Description: "Description 1"
              SearchResult: "SearchResult 1"
              Feature: "Feature 1"
            }
            ListElement {
              FeatureId: 2
              FeatureTitle: "Title 2"
              Description: "Description 2"
              SearchResult: "SearchResult 2"
              Feature: "Feature 2"
            }
            ListElement {
              FeatureId: 3
              FeatureTitle: "Title 3"
              Description: "Description 3"
              SearchResult: "SearchResult 3"
              Feature: "Feature 3"
            }
            ListElement {
              FeatureId: 4
              FeatureTitle: "Title 4"
              Description: "Description 4"
              SearchResult: "SearchResult 4"
              Feature: "Feature 4"
            }
            ListElement {
              FeatureId: 5
              FeatureTitle: "Title 5"
              Description: "Description 5"
              SearchResult: "SearchResult 5"
              Feature: "Feature 5"
            }
            ListElement {
              FeatureId: 6
              FeatureTitle: "Title 6"
              Description: "Description 6"
              SearchResult: "SearchResult 6"
              Feature: "Feature 6"
            }
            ListElement {
              FeatureId: 7
              FeatureTitle: "Title 7"
              Description: "Description 7"
              SearchResult: "SearchResult 7"
              Feature: "Feature 7"
            }
          }
          onFeatureClicked: function(selectedFeatures) {
            preselectedFeatures = selectedFeatures
            text = selectedFeatures.toString()
          }
        }

        MMSliderFormEditor {
          title: "MMSliderEditor"
          from: -100
          to: 100
          parentValue: -100
          suffix: " s"
          width: parent.width
          enabled: checkbox.checked
          onEditorValueChanged: function(newValue) { errorMsg = newValue > 0 ? "" : "Set positive value!" }
          hasCheckbox: true
          checkboxChecked: true
        }

        MMNumberFormEditor {
          title: "MMNumberEditor"
          parentValue: "2.0"
          from: 1.0
          to: 3.0
          width: parent.width
          enabled: checkbox.checked
          precision: 1
          suffix: "s."
          step: Math.pow( 10.0, 0.0 - precision )
          onEditorValueChanged: function(newValue) { parentValue = newValue }
        }

        MMTextInput {
          title: "MMInputEditor"
          text: "Text"
          enabled: checkbox.checked
          width: parent.width
          hasCheckbox: true
          checkboxChecked: false
        }

        MMScannerFormEditor {
          title: "MMQrCodeEditor"
          placeholderText: "QR code"
          warningMsg: text.length > 0 ? "" : "Click to icon and scan the code"
          enabled: checkbox.checked
          width: parent.width

          onEditorValueChanged: function(newValue, isNull) { console.log("QR code: " + newValue) }
        }

        MMPhotoFormEditor {
          title: "MMPhotoEditor"
          width: parent.width
          photoUrl: "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"

          onTrashClicked: console.log("Move to trash")
          onContentClicked: console.log("Open photo")
        }

        MMButtonFormEditor {
          title: "MMButtonInputEditor"
          placeholderText: "Write something"
          text: "Text to copy"
          buttonText: "Copy"
          enabled: checkbox.checked
          width: parent.width
          onButtonClicked: console.log("Copy pressed")
          buttonEnabled: text.length > 0
        }

        MMButtonFormEditor {
          title: "MMButtonInputEditor"
          placeholderText: "Píš"
          buttonText: "Kopíruj"
          enabled: checkbox.checked
          width: parent.width
          buttonEnabled: text.length > 0
        }

        MMTextInput {
          title: "MMInputEditor"
          placeholderText: "Placeholder"
          enabled: checkbox.checked
          width: parent.width
          warningMsg: text.length > 0 ? "" : "Write something"
        }

        MMTextMultilineFormEditor {
          title: "MMTextAreaEditor"
          placeholderText: "Place for multi row text"
          enabled: checkbox.checked
          width: parent.width
          warningMsg: text.length > 0 ? "" : "Write something"
        }

        MMPasswordInput {
          title: "MMPasswordEditor"
          text: "Password"
          //regexp: '(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[^A-Za-z0-9])(?=.{6,})'
          errorMsg: "Password must contain at least 6 characters\nMinimum 1 number, uppercase and lowercase letter and special character"
          enabled: checkbox.checked
          width: parent.width
        }

        MMSwitchFormEditor {
          title: "MMSwitchEditor"
          checked: true
          text: checked ? "True" : "False"
          warningMsg: checked ? "" : "Should be checked :)"
          enabled: checkbox.checked
          width: parent.width
        }
      }
    }
  }
}
