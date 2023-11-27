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

import "../../app/qmlV2/component"
import "../../app/qmlV2/"

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
        anchors.fill: parent

        MMCheckBox {
          id: checkbox
          text: checked ? "enabled" : "disabled"
        }

        MMSliderEditor {
          title: "MMSliderEditor"
          width: 300
          from: -100
          to: 100
          parentValue: -100
          suffix: " s"
          enabled: checkbox.checked
          onEditorValueChanged: function(newValue) { errorMsg = newValue > 0 ? "" : "Set positive value!" }
        }

        MMInputEditor {
          title: "MMInputEditor with Icon"
          parentValue: "Text"
          width: 300
          leftIconSource: StyleV2.calendarIcon
          enabled: checkbox.checked
          onLeftActionClicked: console.log("left icon clicked")
        }

        MMInputEditor {
          title: "MMInputEditor"
          placeholderText: "Placeholder"
          width: 300
          enabled: checkbox.checked
          warningMsg: text.length > 0 ? "" : "Write something"
        }

        MMPasswordEditor {
          title: "MMPasswordEditor"
          parentValue: "Password"
          regexp: '(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[^A-Za-z0-9])(?=.{6,})'
          errorMsg: isPasswordCorrect(text) ? "" : "Password must contain at least 6 characters\nMinimum 1 number, uppercase and lowercase letter and special character"
          enabled: checkbox.checked
        }
      }
    }

    GroupBox {
      title: "MMSliderInput"
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
        anchors.fill: parent
        MMSliderInput {
          title: "Title"
          from: -100
          to: 100
          parentValue: -100
          suffix: " s"

          onEditorValueChanged: function(newValue) { errorMsg = newValue > 0 ? "" : "Set positive value!" }
        }
        MMSliderInput {
          title: "Title"
          enabled: false
          from: 0
          to: 10
          parentValue: 5
        }
      }
    }

  }
}
