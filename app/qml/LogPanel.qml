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
import "."  // import InputStyle singleton
import "components"

Item {
  id: root
  property string text: "(no-entries)"

  signal close

  Keys.onReleased: {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true;
      close();
    }
  }

  Page {
    id: pane
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    clip: true
    height: parent.height
    width: parent.width

    Flickable {
      id: flickableItem
      anchors.horizontalCenter: parent.horizontalCenter
      clip: true
      contentHeight: txt.height
      contentWidth: width
      height: parent.height
      maximumFlickVelocity: __androidUtils.isAndroid ? InputStyle.scrollVelocityAndroid : maximumFlickVelocity
      width: root.width - InputStyle.panelMargin

      Text {
        id: txt
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeNormal
        text: "<style>" + "a:link { color: " + InputStyle.highlightColor + "; text-decoration: underline; }" + "p.odd { color: " + InputStyle.fontColorBright + "; }" + "</style>" + root.text
        textFormat: Text.RichText
        width: parent.width
        wrapMode: Text.WordWrap
      }

      ScrollBar.vertical: ScrollBar {
      }
    }

    background: Rectangle {
      color: "white"
    }
    footer: DelegateButton {
      id: sendButton
      height: InputStyle.rowHeightHeader
      text: __inputHelp.submitReportPending ? qsTr("Sending...") : qsTr("Send to Developers")
      width: root.width

      onClicked: {
        if (!__inputHelp.submitReportPending)
          __inputHelp.submitReport();
      }
    }
    header: PanelHeader {
      id: header
      color: "white"
      height: InputStyle.rowHeightHeader
      rowHeight: InputStyle.rowHeightHeader
      titleText: qsTr("Diagnostic Log")
      width: parent.width
      withBackButton: true

      onBack: root.close()
    }
  }
}
