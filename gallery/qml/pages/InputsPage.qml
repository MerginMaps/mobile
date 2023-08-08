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

import "../../../app/qmlV2/component"

Column {
  padding: 20
  spacing: 20

  GroupBox {
    title: "MMInput"
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
      MMInput {
        placeholderText: "Place holder"
      }
      MMInput {
        text: "Disabled"
        enabled: false
      }
      Column {
        TextInput { text: "type: MMInput.Type.Search" }
        MMInput {
          type: MMInput.Type.Search
          placeholderText: "Search"
        }
      }
      Column {
      TextInput { text: "type: MMInput.Type.Calendar" }
      TextInput { text: "warningMsg: ..." }
        MMInput {
          type: MMInput.Type.Calendar
          text: "Calendar"
          warningMsg: "Would you like to be so kind and select a date please?"
        }
      }
      Column {
      TextInput { text: "type: MMInput.Type.Password" }
      TextInput { text: "errorMsg: ..." }
        MMInput {
          type: MMInput.Type.Password
          text: "Password"
          errorMsg: "Password must contain at least 6 characters\nMinimum 1 number\nAnd 1 special character"
        }
      }
      Column {
      TextInput { text: "type: MMInput.Type.Scan" }
        MMInput {
          type: MMInput.Type.Scan
          text: "QR Code"
        }
      }
      Column {
      TextInput { text: "type: MMInput.Type.CopyButton" }
        MMInput {
          type: MMInput.Type.CopyButton
          text: "Copy me"
        }
      }
      Column {
      TextInput { text: "type: MMInput.Type.CopyButton" }
        MMInput {
          type: MMInput.Type.CopyButton
          text: "Copy me"
          enabled: false
        }
      }
    }
  }
}
