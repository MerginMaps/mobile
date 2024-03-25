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
import "../../app/qml/components"

ScrollView {
  Column {
    padding: 20
    spacing: 20

    MMCheckBox {
      id: checkbox
      text: checked ? "enabled" : "disabled"
      checked: true
    }

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

        MMSearchInput {
          title: "MMSearchInput"
          placeholderText: "Text value"
          onSearchTextChanged: function(text) { console.log("Searched string: " + text) }
        }

        MMSwitchInput {
          title: "MMSwitchInput"
          text: "Visible?"
          enabled: checkbox.checked
          width: parent.width
        }

        MMDropdownInput {
          title: "MMDropdownInput"
          placeholderText: "Select one"
          dropDownTitle: "Select one"
          enabled: checkbox.checked
          width: parent.width
          valueRole: "value"
          textRole: "text"
          dataModel: ListModel {
            ListElement {
              value: 1
              text: "Text 1"
            }
            ListElement {
              value: 2
              text: "Text 2"
            }
            ListElement {
              value: 3
              text: "Text 3"
            }
          }
          onSelectionFinished: function(selectedFeatures) {
            preselectedFeatures = selectedFeatures
            text = selectedFeatures.toString()
          }
        }

        MMDropdownInput {
          title: "MMDropdownInput"
          placeholderText: "Select one"
          dropDownTitle: "Select one"
          enabled: checkbox.checked
          width: parent.width
          dataModel: ListModel {
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
          onSelectionFinished: function(selectedFeatures) {
            preselectedFeatures = selectedFeatures
            text = selectedFeatures.toString()
          }
        }

        MMDropdownInput {
          title: "MMDropdownInput Multi select"
          placeholderText: "Select multiple"
          dropDownTitle: "Multi select"
          enabled: checkbox.checked
          width: parent.width
          multiSelect: true
          withSearchbar: true
          preselectedFeatures: []

          dataModel: ListModel {
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
          onSelectionFinished: function(selectedFeatures) {
            preselectedFeatures = selectedFeatures
            text = selectedFeatures.toString()
          }
        }

        MMTextInput {
          title: "MMTextInput"
          text: "Text"
          enabled: checkbox.checked
          width: parent.width
          hasCheckbox: true
          checkboxChecked: false
        }

        MMTextInput {
          title: "MMTextInput"
          placeholderText: "Placeholder"
          enabled: checkbox.checked
          width: parent.width
          warningMsg: text.length > 0 ? "" : "Write something"
        }

        MMTextWithButtonInput {
          title: "MMTextWithButtonInput"
          placeholderText: "Write something"
          buttonText: "Copy"
          width: parent.width
          onButtonClicked: console.log("Copy pressed")
          buttonEnabled: text.length > 0
        }

        MMPasswordInput {
          title: "MMPasswordInput"
          text: "Password"
          //regexp: '(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[^A-Za-z0-9])(?=.{6,})'
          errorMsg: "Password must contain at least 6 characters\nMinimum 1 number, uppercase and lowercase letter and special character"
          enabled: checkbox.checked
          width: parent.width
        }
      }
    }
  }
}
