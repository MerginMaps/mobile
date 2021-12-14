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
import QtQuick.Window 2.12

import "."  // import InputStyle singleton
import "./components"

Page {
  id: root

  property real fieldHeight: InputStyle.rowHeight
  property real panelMargin: fieldHeight / 4
  property color fontColor: "white"
  property color bgColor: InputStyle.fontColor
  property bool isPortraitOrientation: Screen.primaryOrientation === Qt.PortraitOrientation

  signal close()

  function setAnchors() {
    if ( isPortraitOrientation )
    {
      lutraLogo.anchors.right = undefined
      lutraLogo.anchors.horizontalCenter = content.horizontalCenter
      lutraLogo.anchors.bottomMargin = 2 * InputStyle.panelMargin
      lutraLogo.anchors.rightMargin = undefined
    }
    else // landscape
    {
      lutraLogo.anchors.horizontalCenter = undefined
      lutraLogo.anchors.right = content.right
      lutraLogo.anchors.bottomMargin = InputStyle.panelMargin
      lutraLogo.anchors.rightMargin = InputStyle.panelMargin
    }
  }

  Keys.onReleased: {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      close()
    }
  }

  onIsPortraitOrientationChanged: setAnchors()
  Component.onCompleted: setAnchors()

  background: Rectangle {
    color: root.bgColor
  }

  header: PanelHeader {
    id: header
    height: InputStyle.rowHeightHeader
    width: parent.width
    color: root.bgColor
    rowHeight: InputStyle.rowHeightHeader
    titleText: ""

    onBack: root.close()
    withBackButton: true
  }

  Item {
    id: content
    anchors.fill: parent
    anchors.bottomMargin: Qt.inputMethod.keyboardRectangle.height ? Qt.inputMethod.keyboardRectangle.height : 0

    Column {
      id: columnLayout
      anchors.verticalCenter: parent.verticalCenter
      width: parent.width

      Image {
        id: inputLogo
        source: InputStyle.inputLogo
        width: content.width / 2
        sourceSize.width: width
        anchors.horizontalCenter: parent.horizontalCenter
      }

      Text {
        text: "v" + __version
        font.pixelSize: InputStyle.fontPixelSizeSmall
        anchors.horizontalCenter: parent.horizontalCenter
        color: fontColor
      }

      Button {
        id: inputLinkBtn
        width: content.width - 2 * root.panelMargin
        height: fieldHeight * 0.7
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: Qt.openUrlExternally( __inputHelp.inputWebLink )
        background: Rectangle {
          color: InputStyle.fontColor
        }

        contentItem: Text {
          text: __inputHelp.inputWebLink.split( '?' )[0] // do not show utm tags
          font.pixelSize: InputStyle.fontPixelSizeNormal
          color: InputStyle.highlightColor
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideRight
        }
      }
    }

    Text {
      id: developedText
      text: qsTr("Developed by")
      font.pixelSize: InputStyle.fontPixelSizeSmall
      anchors.horizontalCenter: lutraLogo.horizontalCenter
      anchors.bottom: lutraLogo.top
      color: fontColor
    }

    Image {
      id: lutraLogo
      source: InputStyle.lutraLogo
      width: inputLogo.width / 2
      sourceSize.width: width
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: parent.bottom
      anchors.bottomMargin: InputStyle.panelMargin * 2
    }

    ColorOverlay {
      anchors.fill: lutraLogo
      source: lutraLogo
      color: fontColor
    }
  }
}
