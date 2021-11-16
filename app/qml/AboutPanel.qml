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
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton
import "components"

Page {
  id: root
  property color bgColor: InputStyle.fontColor
  property real fieldHeight: InputStyle.rowHeight
  property color fontColor: "white"
  property bool isPortraitOrientation: Screen.primaryOrientation === Qt.PortraitOrientation
  property real panelMargin: fieldHeight / 4

  signal close
  function setAnchors() {
    if (isPortraitOrientation) {
      lutraLogo.anchors.right = undefined;
      lutraLogo.anchors.horizontalCenter = content.horizontalCenter;
      lutraLogo.anchors.bottomMargin = 2 * InputStyle.panelMargin;
      lutraLogo.anchors.rightMargin = undefined;
    } else // landscape
    {
      lutraLogo.anchors.horizontalCenter = undefined;
      lutraLogo.anchors.right = content.right;
      lutraLogo.anchors.bottomMargin = InputStyle.panelMargin;
      lutraLogo.anchors.rightMargin = InputStyle.panelMargin;
    }
  }

  Component.onCompleted: setAnchors()
  Keys.onReleased: {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true;
      close();
    }
  }
  onIsPortraitOrientationChanged: setAnchors()

  Item {
    id: content
    anchors.bottomMargin: Qt.inputMethod.keyboardRectangle.height ? Qt.inputMethod.keyboardRectangle.height : 0
    anchors.fill: parent

    Column {
      id: columnLayout
      anchors.verticalCenter: parent.verticalCenter
      width: parent.width

      Image {
        id: inputLogo
        anchors.horizontalCenter: parent.horizontalCenter
        source: InputStyle.inputLogo
        sourceSize.width: width
        width: content.width / 2
      }
      Text {
        anchors.horizontalCenter: parent.horizontalCenter
        color: fontColor
        font.pixelSize: InputStyle.fontPixelSizeSmall
        text: "v" + __version
      }
      Button {
        id: inputLinkBtn
        anchors.horizontalCenter: parent.horizontalCenter
        height: fieldHeight * 0.7
        width: content.width - 2 * root.panelMargin

        onClicked: Qt.openUrlExternally(__inputHelp.inputWebLink)

        background: Rectangle {
          color: InputStyle.fontColor
        }
        contentItem: Text {
          color: InputStyle.highlightColor
          elide: Text.ElideRight
          font.pixelSize: InputStyle.fontPixelSizeNormal
          horizontalAlignment: Text.AlignHCenter
          text: __inputHelp.inputWebLink.split('?')[0] // do not show utm tags
          verticalAlignment: Text.AlignVCenter
        }
      }
    }
    Text {
      id: developedText
      anchors.bottom: lutraLogo.top
      anchors.horizontalCenter: lutraLogo.horizontalCenter
      color: fontColor
      font.pixelSize: InputStyle.fontPixelSizeSmall
      text: qsTr("Developed by")
    }
    Image {
      id: lutraLogo
      anchors.bottom: parent.bottom
      anchors.bottomMargin: InputStyle.panelMargin * 2
      anchors.horizontalCenter: parent.horizontalCenter
      source: InputStyle.lutraLogo
      sourceSize.width: width
      width: inputLogo.width / 2
    }
    ColorOverlay {
      anchors.fill: lutraLogo
      color: fontColor
      source: lutraLogo
    }
  }

  background: Rectangle {
    color: root.bgColor
  }
  header: PanelHeader {
    id: header
    color: root.bgColor
    height: InputStyle.rowHeightHeader
    rowHeight: InputStyle.rowHeightHeader
    titleText: ""
    width: parent.width
    withBackButton: true

    onBack: root.close()
  }
}
