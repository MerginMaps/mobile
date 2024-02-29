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

import "../../app/qml/components"

Item {

  focus: true

  MMPage {

    anchors.fill: parent

    pageHeader.title: "MMPage component 🤩"

    content: Item {
      width: parent.width
      height: parent.height

      Text {
        anchors {
          fill: parent
          margins: 20
        }

        wrapMode: Text.WordWrap
        textFormat: Text.RichText

        text: "<b>About MMPage component</b><br>" +
              " - component takes full horizontal and vertical space <br>" +
              " - can serve as base class for all simple pages <br>" +
              " - default header - <code>MMPageHeader</code>, it is exposed via <code>pageHeader</code> property <br>" +
              " - safe margins, page margins and maximum page width are all incorporated <br>" +
              " - content does not scroll automatically <br>" +
              " <br> " +
              "<b>How to use:</b><br>" +
              " - Add content via <code>content</code> property as <code>content: Item { anchors.fill: parent; ...}</code><br>" +
              " - Add footer as <code>footer: MMFooter {...}</code> <br>" +
              " - Change title (or other header properties) via <code>pageHeader.title = \"title\"<br>"
      }
    }
  }
}
