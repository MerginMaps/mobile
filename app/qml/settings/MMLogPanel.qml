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
import Qt5Compat.GraphicalEffects

import "../components"

Item {
  id: root
  property string text: "(no-entries)"
  property bool enableSendToDev: true
  property bool submitReportPending: __inputHelp.submitReportPending

  signal submitReport
  signal close

  Keys.onReleased: function( event ) {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      close()
    }
  }

  Page {
    id: pane

    width: parent.width
    height: parent.height
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    clip: true

    background: Rectangle {
      color: __style.lightGreenColor
    }

    header: MMHeader {
      id: header
      title: qsTr("Diagnostic log")
      titleFont: __style.h3

      onBackClicked: root.close()
      backVisible: true
    }

    Flickable {
      id: flickableItem
      clip: true
      anchors.horizontalCenter: parent.horizontalCenter
      width: root.width - __style.pageMargins * 2
      height: parent.height
      contentHeight: txt.height
      contentWidth: width
      maximumFlickVelocity: __androidUtils.isAndroid ? __style.scrollVelocityAndroid : maximumFlickVelocity

      Text {
        id: txt
        text: "<style>" + "a:link { color: " + InputStyle.highlightColor
              + "; text-decoration: underline; }" + "p.odd { color: "
              + __style.nightColor + "; }" + "</style>" + root.text
        font: __style.t3
        color: __style.forestColor
        textFormat: Text.RichText
        wrapMode: Text.WordWrap
        width: parent.width
      }

      ScrollBar.vertical: ScrollBar { }
    }
  }

  MMButton {
        id: sendButton
        visible: enableSendToDev
        anchors.bottom: root.bottom
        anchors.bottomMargin: 32 * __dp
        anchors.horizontalCenter: parent.horizontalCenter

        width: root.width - __style.pageMargins * 2
        text: root.submitReportPending ? qsTr("Sending...") : qsTr("Send")

        onClicked: {
          if (!root.submitReportPending)
            root.submitReport()
        }
  }
}
