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

import "../../app/qml/components"
import "../"

ScrollView {
  id: root

  property alias testText: textInput.text

  Column {
    padding: 20
    spacing: 5

    Text {
      text: "MMText"
      font.pixelSize: 40
    }

    Text {
      text: "Type test text"
      font.bold: true
    }

    Rectangle {
      color: "white"
      width: textInput.width
      height: textInput.height
      TextInput {
        id: textInput
        text: "ABCDEFGHIJK abcdefghij 1234567890";
      }
    }

    Rectangle {
      color: "gray"
      width: root.width
      height: 1
    }

    Item {
      width: 1
      height: 10
    }

    TextItem {
      testText: root.testText
      text: "custom font (no mm-font)"
    }

    TextItem {
      testText: root.testText
      text: "h1"
      font: __style.h1
    }

    TextItem {
      testText: root.testText
      text: "h2"
      font: __style.h2
    }

    TextItem {
      testText: root.testText
      text: "h3"
      font: __style.h3
    }


    TextItem {
      testText: root.testText
      text: "t1"
      font: __style.t1
    }

    TextItem {
      testText: root.testText
      text: "t2"
      font: __style.t2
    }

    TextItem {
      testText: root.testText
      text: "t3"
      font: __style.t3
    }

    TextItem {
      testText: root.testText
      text: "t4"
      font: __style.t4
    }

    TextItem {
      testText: root.testText
      text: "t5"
      font: __style.t5
    }

    TextItem {
      testText: root.testText
      text: "p1"
      font: __style.p1
    }

    TextItem {
      testText: root.testText
      text: "p2"
      font: __style.p2
    }

    TextItem {
      testText: root.testText
      text: "p3"
      font: __style.p3
    }

    TextItem {
      testText: root.testText
      text: "p4"
      font: __style.p4
    }

    TextItem {
      testText: root.testText
      text: "p5"
      font: __style.p5
    }

    TextItem {
      testText: root.testText
      text: "p6"
      font: __style.p6
    }

    TextItem {
      testText: root.testText
      text: "p7"
      font: __style.p7
    }
  }
}
