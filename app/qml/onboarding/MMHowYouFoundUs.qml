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
import "../inputs"

import notificationType 1.0

Page {
  id: root

  width: parent.width

  property string selectedText: ""

  signal backClicked
  signal howYouFoundUsSelected(var selectedText)

  readonly property string headerTitle: qsTr("How did you learn about us?")
  readonly property real hPadding: width < __style.maxPageWidth
                                   ? 20 * __dp
                                   : (20 + (width - __style.maxPageWidth) / 2) * __dp

  readonly property string specifySourceText: qsTr("Please specify the source")

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
    step: 2

    onBackClicked: root.backClicked()
  }

  Item {
    width: parent.width
    height: parent.height - (listView.model.count === listView.currentIndex + 1 ? header.height + 50 * __dp : 0)
    anchors.top: header.bottom
    anchors.topMargin: 20 * __dp

    ListView {
      id: listView

      width: parent.width - 2 * root.hPadding
      anchors.horizontalCenter: parent.horizontalCenter
      height: parent.height - header.height
      spacing: 10 * __dp
      clip: true

      Component.onCompleted: currentIndex = -1

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

        onClicked: {

          listView.currentIndex = index

          if(listView.model.count === listView.currentIndex + 1)
          {
            root.selectedText = ""
            listView.positionViewAtEnd()
          }
          else {
            if (model.icon === __style.socialMediaIcon) {
              // need to select subcategory
              root.selectedText = ""
            } else {
              root.selectedText = model.name
            }
          }
        }
      }

      footer: Column {
        width: root.width - 2 * root.hPadding
        topPadding: 20 * __dp
        visible: listView.model.count === listView.currentIndex + 1 // === Other

        MMInputEditor {
          id: otherSourceText
          title: qsTr("Source")
          placeholderText: root.specifySourceText
          onTextChanged: root.selectedText = text
          onVisibleChanged: if(visible) hasFocus = true
        }

        Item { width: 1; height: 60 * __dp }
      }
    }
  }

  MMButton {
    width: root.width - 2 * root.hPadding
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 20 * __dp
    text: qsTr("Continue")

    onClicked: {
      if (root.selectedText.length > 0 ) {
        root.howYouFoundUsSelected(root.selectedText)
      } else {
        __notificationModel.add(
          root.specifySourceText,
          3,
          NotificationType.Error,
          NotificationType.None
        )
      }
    }
  }
}
