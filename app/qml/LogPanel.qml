/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Item {
    id: root
    property string text: "(no-entries)"

    function close() {
        visible = false
    }

    Pane {
        id: pane

        width: parent.width
        height: parent.height
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        clip: true

        background: Rectangle {
            color: "white"
        }

        PanelHeader {
          id: header
          height: InputStyle.rowHeightHeader
          width: parent.width
          color: "white"
          rowHeight: InputStyle.rowHeightHeader
          titleText: qsTr("Diagnostic Log")

          onBack: root.close()
          withBackButton: true
        }

        Flickable {
          id: flickableItem
          anchors.top: header.bottom
          clip: true
          anchors.horizontalCenter: parent.horizontalCenter
          width: root.width - 2 * InputStyle.rowHeightHeader
          height: parent.height - 2 * InputStyle.rowHeightHeader - InputStyle.panelSpacing
          contentHeight: txt.height
          contentWidth: width

            Text {
              id: txt
              text: "<style>" +
                      "a:link { color: " + InputStyle.highlightColor + "; text-decoration: underline; }" +
                      "p.odd { color: " + InputStyle.fontColorBright + "; }" +
                    "</style>"

                    + root.text
              font.pixelSize: InputStyle.fontPixelSizeNormal
              color: InputStyle.fontColor
              textFormat: Text.RichText
              wrapMode: Text.WordWrap
              width: parent.width
            }

            ScrollBar.vertical: ScrollBar { id: vbar }
        }

        Button {
          anchors.bottom: parent.bottom
          id: sendButton
          width: root.width - 2 * InputStyle.rowHeightHeader
          anchors.horizontalCenter: parent.horizontalCenter

          height: InputStyle.rowHeightHeader
          text: __inputHelp.submitReportPending ? qsTr("Sending...") : qsTr("Send Log to Developers")
          font.pixelSize: sendButton.height / 3

          background: Rectangle {
            color: InputStyle.fontColor
          }

          onClicked: {
            if (!__inputHelp.submitReportPending)
              __inputHelp.submitReport();
          }

          contentItem: Text {
            text: sendButton.text
            font: sendButton.font
            opacity: enabled ? 1.0 : 0.3
            color: "white"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
          }
        }
    }
}
