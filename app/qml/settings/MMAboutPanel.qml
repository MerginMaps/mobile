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
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Window

import "../components"

Page {
  id: root

  signal close()
  signal visitWebsiteClicked()

  Keys.onReleased: function( event ) {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      close()
    }
  }

  background: Rectangle {
    color: __style.lightGreenColor
  }

  header: MMHeader {
    id: header
    title: qsTr("About Mergin Maps")
    titleFont: __style.t3

    onBackClicked: root.close()
    backVisible: true
  }

  Item {
    id: content
    anchors.fill: parent

    Column {
      anchors.verticalCenter: parent.verticalCenter
      width: parent.width
      spacing: __style.pageMargins

      Text {
        text: "v" + __version
        font: __style.t4
        color: __style.deepOceanColor
        anchors.horizontalCenter: parent.horizontalCenter
      }

      Image {
        id: mmLogo
        source: __style.mmLogoImage
        anchors.horizontalCenter: parent.horizontalCenter
      }

      Text {
        text: qsTr("We are bringing the benefits of open source GIS to businesses without compromises")
        font: __style.p5
        color: __style.nightColor
        wrapMode: Text.WordWrap
        width: webLinkBtn.width
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
      }

      MMButton {
        id: webLinkBtn
        width: Math.min(__style.maxPageWidth, content.width - 2* __style.pageMargins)
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: root.visitWebsiteClicked()
        text: qsTr("Visit website")
      }
    }
  }

  footer: Column {
    spacing: __style.pageMargins
    bottomPadding: __style.pageMargins

    Text {
      id: developedText
      text: qsTr("Developed by")
      font: __style.t4
      color: __style.deepOceanColor
      anchors.horizontalCenter: lutraLogo.horizontalCenter
    }

    Image {
      id: lutraLogo
      source: __style.lutraLogoImage
      anchors.horizontalCenter: parent.horizontalCenter
    }
  }
}
