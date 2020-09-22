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
            color: InputStyle.bgColor
        }

        PanelHeader {
          id: header
          height: InputStyle.rowHeightHeader
          width: parent.width
          color: "white"
          rowHeight: InputStyle.rowHeightHeader
          titleText: qsTr("Detailed Log")

          onBack: root.close()
          withBackButton: true
        }

        Flickable {
          anchors.top: header.bottom
          clip: true
          anchors.horizontalCenter: parent.horizontalCenter
          width: root.width - 2 * InputStyle.rowHeightHeader
          height: parent.height - InputStyle.rowHeightHeader
          contentHeight: txt.height
          contentWidth: width

            Text {
              id: txt
              text: root.text
              font.pixelSize: InputStyle.fontPixelSizeNormal
              color: InputStyle.fontColor
              textFormat: Text.RichText
              wrapMode: Text.WordWrap
              width: parent.width
            }

            ScrollBar.vertical: ScrollBar { id: vbar; active: hbar.active }
        }
    }
}
