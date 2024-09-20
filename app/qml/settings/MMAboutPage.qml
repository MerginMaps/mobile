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

import "../components"

MMPage {
  id: root

  signal visitWebsiteClicked()

  pageHeader.title: qsTr( "About the App" )

  pageContent: ScrollView {

    width: parent.width
    height: parent.height

    contentWidth: availableWidth // to only scroll vertically
    ScrollBar.vertical.policy: ScrollBar.AlwaysOff
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    ColumnLayout {
      width: parent.width
      height: parent.height


      Item {
        Layout.preferredHeight: __style.margin40
        Layout.fillWidth: true
      }

      Column {
        id: middleContentGroup

        Layout.fillWidth: true
        Layout.preferredHeight: childrenRect.height

        spacing: __style.spacing20

        Text {
          id: vText

          text: "v" + __version
          font: __style.t4
          color: __style.deepOceanColor
          anchors.horizontalCenter: parent.horizontalCenter
        }

        Image {
          id: mmLogo

          source: __style.mmLogoImage
          sourceSize.width: 150 * __dp
          anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
          id: descText

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

          width: parent.width
          anchors.horizontalCenter: parent.horizontalCenter
          onClicked: root.visitWebsiteClicked()
          text: qsTr("Visit website")
        }
      }

      Item {
        Layout.preferredHeight: __style.margin40
        Layout.fillWidth: true
      }

      Column {
        Layout.fillWidth: true
        Layout.preferredHeight: childrenRect.height

        spacing: 0

        Text {
          id: developedText
          text: qsTr( "Developed by" )
          font: __style.t4
          color: __style.deepOceanColor
          anchors.horizontalCenter: lutraLogo.horizontalCenter
        }

        Image {
          id: lutraLogo
          source: __style.lutraLogoImage
          anchors.horizontalCenter: parent.horizontalCenter

          height: 80 * __dp
          sourceSize.height: height
        }
      }
    }
  }
}
