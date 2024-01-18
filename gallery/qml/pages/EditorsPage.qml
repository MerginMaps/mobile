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

    GroupBox {
      title: "Items based on MMAbstractEditor"
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

        MMSliderEditor {
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

        MMNumberEditor {
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

        MMInputEditor {
          title: "MMInputEditor"
          parentValue: "Text"
          enabled: checkbox.checked
          width: parent.width
          hasCheckbox: true
          checkboxChecked: false
        }

        MMQrCodeEditor {
          title: "MMQrCodeEditor"
          placeholderText: "QR code"
          warningMsg: text.length > 0 ? "" : "Click to icon and scan the code"
          enabled: checkbox.checked
          width: parent.width

          onEditorValueChanged: function(newValue, isNull) { console.log("QR code: " + newValue) }
        }

        MMButtonInputEditor {
          title: "MMButtonInputEditor"
          placeholderText: "Write something"
          text: "Text to copy"
          buttonText: "Copy"
          enabled: checkbox.checked
          width: parent.width
          onButtonClicked: console.log("Copy pressed")
          buttonEnabled: text.length > 0
        }

        MMButtonInputEditor {
          title: "MMButtonInputEditor"
          placeholderText: "Píš"
          buttonText: "Kopíruj"
          enabled: checkbox.checked
          width: parent.width
          buttonEnabled: text.length > 0
        }

        MMInputEditor {
          title: "MMInputEditor"
          placeholderText: "Placeholder"
          enabled: checkbox.checked
          width: parent.width
          warningMsg: text.length > 0 ? "" : "Write something"
        }

        MMTextAreaEditor {
          title: "MMTextAreaEditor"
          placeholderText: "Place for multi row text"
          enabled: checkbox.checked
          width: parent.width
          warningMsg: text.length > 0 ? "" : "Write something"
        }

        MMPasswordEditor {
          title: "MMPasswordEditor"
          text: "Password"
          //regexp: '(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[^A-Za-z0-9])(?=.{6,})'
          errorMsg: "Password must contain at least 6 characters\nMinimum 1 number, uppercase and lowercase letter and special character"
          enabled: checkbox.checked
          width: parent.width
        }

        MMSwitchEditor {
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
