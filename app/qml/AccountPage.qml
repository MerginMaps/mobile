/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Page {
  property color bgColor: "white"
  property string username: __merginApi.username
  property int storageLimit: __merginApi.storageLimit
  property int diskUsage: __merginApi.diskUsage

  id: root
  visible: true

  function formatNumber(number) {
    return number / (1024 * 1024)
  }

  // header
  PanelHeader {
    id: header
    height: InputStyle.rowHeightHeader
    width: parent.width
    color: InputStyle.clrPanelMain
    rowHeight: InputStyle.rowHeightHeader
    titleText: qsTr("Account")

    onBack: root.visible = false
    withBackButton: true
  }

  // avatar
  Item {
    id: avatarContainer
    width: parent.width
    height: InputStyle.rowHeightHeader * 4
    anchors.top: header.bottom

    Item {
      id: avatar
      width: avatarContainer.height
      height: width
      anchors.centerIn: parent
      anchors.horizontalCenter: parent.horizontalCenter

      Rectangle {
        id: avatarImage
        anchors.centerIn: parent
        width: avatar.width * 0.8
        height: width
        color: InputStyle.fontColor
        radius: width*0.5
        antialiasing: true

        Image {
          id: userIcon
          anchors.centerIn: parent
          source: 'account.svg'
          height: parent.height * 0.8
          width: height
          sourceSize.width: width
          sourceSize.height: height
          fillMode: Image.PreserveAspectFit
        }

        ColorOverlay {
          anchors.fill: userIcon
          source: userIcon
          color: "#FFFFFF"
        }
      }
    }

    Text {
      text: root.username
      anchors.horizontalCenter: parent.horizontalCenter
      horizontalAlignment: Text.AlignHCenter
      color: InputStyle.panelBackgroundDarker
      font.pixelSize: InputStyle.fontPixelSizeTitle * 2
      anchors.top: avatar.bottom
    }
  }

  // Stats
  Rectangle {
    anchors.top: avatarContainer.bottom
    width: parent.width
    height: InputStyle.rowHeightHeader * 4
    anchors.margins: InputStyle.panelMargin * 3
    color: root.bgColor

    Column {
      width: parent.width
      height: parent.height
      padding: InputStyle.panelMargin
      spacing: 10 * QgsQuick.Utils.dp

      Text  {
        text:  qsTr("Used Data")
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.panelBackgroundDarker
      }

      ProgressBar {
        property real itemHeight: InputStyle.fontPixelSizeNormal

        id: progressBar
        width: parent.width - parent.padding*2
        height: itemHeight
        value: root.diskUsage/root.storageLimit

        background: Rectangle {
          implicitWidth: parent.width
          implicitHeight: progressBar.itemHeight
          color: InputStyle.panelBackgroundLight
        }

        contentItem: Item {
          implicitWidth: parent.width
          implicitHeight: progressBar.itemHeight

          Rectangle {
            width: progressBar.visualPosition * parent.width
            height: parent.height
            color: InputStyle.fontColor
          }
        }
      }

      Text  {
        text:  formatNumber(root.diskUsage).toFixed(2) + "/" +  formatNumber(root.storageLimit) + "   MB"
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor
      }
    }
  }

  Button {
    id: signOutButton
    height: InputStyle.fontPixelSizeTitle
    text: qsTr("Sign out")
    font.pixelSize: signOutButton.height
    font.bold: true
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.bottomMargin: signOutButton.height * 3
    background: Rectangle {
      color: root.bgColor
    }

    onClicked: {
      if (__merginApi.hasAuthData()) {
          __merginApi.clearAuth()
          root.visible = false
      }
    }

    contentItem: Text {
      text: signOutButton.text
      font: signOutButton.font
      color: InputStyle.highlightColor
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      elide: Text.ElideRight
    }
  }


}
