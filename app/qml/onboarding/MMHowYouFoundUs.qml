/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "../components"

Page {
  id: root

  width: parent.width

  signal backClicked
  signal continueClicked

  readonly property string headerTitle: qsTr("How did you learn about us?")
  readonly property real hPadding: width < __style.maxPageWidth
                                   ? 20 * __dp
                                   : (20 + (width - __style.maxPageWidth) / 2) * __dp

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  MMHeader {
    id: header

    x: root.hPadding
    y: 20 * __dp
    width: root.width - 2 * root.hPadding
    headerTitle: listView.contentY > -30 * __dp ? root.headerTitle : ""
    backVisible: false
    step: 2

    onBackClicked: root.backClicked()
  }

  Item {
    width: parent.width
    height: parent.height - header.height - 40 * __dp
    anchors.top: header.bottom
    anchors.topMargin: 20 * __dp

    ListView {
      id: listView

      width: parent.width - 2 * root.hPadding
      anchors.horizontalCenter: parent.horizontalCenter
      height: parent.height - header.height
      spacing: 10 * __dp
      clip: true

      model: ListModel {
        Component.onCompleted: {
          listView.model.append({name: qsTr("Search engine (Google, ...)"), icon: __style.searchIcon, submenu: false})
          listView.model.append({name: qsTr("Blog"), icon: __style.termsIcon, submenu: false})
          listView.model.append({name: qsTr("Mouth"), icon: __style.mouthIcon, submenu: false})
          listView.model.append({name: qsTr("QGIS website"), icon: __style.qgisIcon, submenu: false})
          listView.model.append({name: qsTr("Application store"), icon: __style.subscriptionsIcon, submenu: false})
          listView.model.append({name: qsTr("Teacher"), icon: __style.teacherIcon, submenu: false})
          listView.model.append({name: qsTr("Conference"), icon: __style.briefcaseIcon, submenu: false})
          listView.model.append({name: qsTr("Social media"), icon: __style.socialMediaIcon, submenu: false})
          listView.model.append({name: qsTr("YouTube"), icon: __style.youtubeIcon, submenu: true})
          listView.model.append({name: qsTr("Twitter"), icon: __style.xTwitterIcon, submenu: true})
          listView.model.append({name: qsTr("Facebook"), icon: __style.facebookIcon, submenu: true})
          listView.model.append({name: qsTr("LinkedIn"), icon: __style.linkedinIcon, submenu: true})
          listView.model.append({name: qsTr("Mastodon"), icon: __style.mastodonIcon, submenu: true})
          listView.model.append({name: qsTr("Reddit"), icon: __style.redditIcon, submenu: true})
          listView.model.append({name: qsTr("Other"), icon: __style.otherIcon, submenu: false})
        }
      }

      header: Text {
        id: listHeader

        width: root.width - 2 * root.hPadding
        padding: 20 * __dp
        text: root.headerTitle
        font: __style.h3
        color: __style.forestColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        lineHeight: 1.2
      }

      delegate: MMIconCheckBoxHorizontal {
        width: model.submenu ? listView.width - 20 * __dp : listView.width
        x: model.submenu ? 20 * __dp : 0
        sourceIcon: model.icon
        text: model.name
        small: model.submenu
        checked: listView.currentIndex === index

        onClicked: listView.currentIndex = index
      }
    }
  }

  MMButton {
    width: root.width - 2 * root.hPadding
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 20 * __dp
    text: qsTr("Continue")

    onClicked: root.continueClicked()
  }
}
