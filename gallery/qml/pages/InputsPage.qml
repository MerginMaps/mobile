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

ScrollView {
  Column {
    padding: 20
    spacing: 20

    CheckBox {
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
