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
import "./components"

Item {
  id: root
  property string text: "(no-entries)"
  property bool enableSendToDev: true

  signal close

  Keys.onReleased: {
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
      color: "white"
    }

    header: PanelHeader {
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
      clip: true
      anchors.horizontalCenter: parent.horizontalCenter
      width: root.width - InputStyle.panelMargin
      height: parent.height
      contentHeight: txt.height
      contentWidth: width
      maximumFlickVelocity: __androidUtils.isAndroid ? InputStyle.scrollVelocityAndroid : maximumFlickVelocity

      Text {
        id: txt
        text: "<style>" + "a:link { color: " + InputStyle.highlightColor
              + "; text-decoration: underline; }" + "p.odd { color: "
              + InputStyle.fontColorBright + "; }" + "</style>" + root.text
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor
        textFormat: Text.RichText
        wrapMode: Text.WordWrap
        width: parent.width
      }

      ScrollBar.vertical: ScrollBar { }
    }

    footer: DelegateButton {
      id: sendButton
      visible: enableSendToDev

      width: root.width
      height: InputStyle.rowHeightHeader
      text: __inputHelp.submitReportPending ? qsTr("Sending...") : qsTr("Send to Developers")

      onClicked: {
        if (!__inputHelp.submitReportPending)
          __inputHelp.submitReport()
      }
    }
  }
}
