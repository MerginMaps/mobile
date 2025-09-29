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

import "../components"

MMPage {
  id: root

  property string text: qsTr( "(no-entries)" )

  property bool enableSendToDev: true
  property bool submitReportPending: __inputHelp.submitReportPending

  signal submitReport()

  pageHeader.title: qsTr( "Diagnostic log" )

  pageContent: Item {
    width: parent.width
    height: parent.height

    Flickable {
      width: parent.width
      height: parent.height

      contentWidth: width
      contentHeight: txt.height + __style.margin40 + sendButton.height

      maximumFlickVelocity: __androidUtils.isAndroid ? __style.scrollVelocityAndroid : maximumFlickVelocity

      TextEdit {
        id: txt
        text: "Kaustuv <style>" + "a:link { color: " + __style.earthColor
              + "; text-decoration: underline; }" + "p.odd { color: "
              + __style.nightColor + "; }" + "</style>" + root.text + "KaustuvLAST"
        font: __style.t3
        color: __style.forestColor
        textFormat: Text.RichText
        wrapMode: Text.WordWrap
        readOnly: true
        selectByMouse: true
        selectByKeyboard: true
        width: parent.width
      }

      ScrollBar.vertical: ScrollBar { }
    }

    MMButton {
      id: sendButton

      visible: root.enableSendToDev

      anchors.bottom: parent.bottom
      anchors.bottomMargin: __style.margin8 + __style.safeAreaBottom

      width: parent.width

      text: root.submitReportPending ? qsTr( "Sending..." ) : qsTr( "Send to developers" )

      onClicked: {
        if ( !root.submitReportPending )
          root.submitReport()
      }
    }
  }
}
