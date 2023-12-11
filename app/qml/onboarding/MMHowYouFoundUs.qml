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

  signal backClicked
  signal continueClicked


  ListModel {
      id: howYouFoundUsModel

      /*
      ListElement {
          channel: qsTr("Search engine (Google, ...)")
          //channelIcon: __style.searchIcon
      }
      ListElement {
        channel: qsTr("Social Media")
        //channelIcon: __style.socialMediaIcon
        details: [
              ListElement {
                media: "YouTube"
                //mediaIcon: __style.youtubeIcon
              },
              ListElement {
                media: "X (formerly Twitter)"
                //mediaIcon: __style.xTwitterIcon
              }
          ]
      }
      ListElement {
          channel: qsTr("Other")
          // channelIcon: __style.otherIcon
      }

      */
  }

  Column {
    MMHeader {
      headerTitle: qsTr("How did you learn about us")
      step: 2

      onBackClicked: root.backClicked()
    }

    TreeView {
            model: howYouFoundUsModel

            delegate: Item {
                id: treeDelegate

                implicitWidth: padding + label.x + label.implicitWidth + padding
                implicitHeight: label.implicitHeight * 1.5

                readonly property real indent: 20
                readonly property real padding: 5

                // Assigned to by TreeView:
                required property TreeView treeView
                required property bool isTreeNode
                required property bool expanded
                required property int hasChildren
                required property int depth

                TapHandler {
                    onTapped: treeView.toggleExpanded(row)
                }

                Text {
                    id: indicator
                    visible: treeDelegate.isTreeNode && treeDelegate.hasChildren
                    x: padding + (treeDelegate.depth * treeDelegate.indent)
                    anchors.verticalCenter: label.verticalCenter
                    text: "▸"
                    rotation: treeDelegate.expanded ? 90 : 0
                }

                Text {
                    id: label
                    x: padding + (treeDelegate.isTreeNode ? (treeDelegate.depth + 1) * treeDelegate.indent : 0)
                    width: treeDelegate.width - treeDelegate.padding - x
                    clip: true
                    text: model.channel
                }
            }
        }

    MMButton {
      onClicked: root.continueClicked()
      text: qsTr("Continue")
    }
  }
}
