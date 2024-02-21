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

import lc 1.0
import "../components"

Page {
  id: statusPanel

  property real rowHeight: __style.row49
  property bool hasChanges: true

  signal back()

  header: MMHeader {
    id: header

    width: parent.width
    color: __style.lightGreenColor
    title: statusPanel.hasChanges ? qsTr("Your local changes") : qsTr("Project Status")
    z: contentLayout.z + 1
    onBackClicked: {
      statusPanel.back()
    }
  }

  background: Rectangle {
    color: __style.lightGreenColor
  }

  // No changes content
  Column {
    id: noChangesContent

    visible: !statusPanel.hasChanges
    anchors.verticalCenter: parent.verticalCenter
    width: statusPanel.width
    spacing: __style.margin12

    Image {
      anchors.horizontalCenter: parent.horizontalCenter
      source: __style.syncImage
    }

    Text {
      anchors.horizontalCenter: parent.horizontalCenter
      horizontalAlignment: Text.AlignHCenter
      width: parent.width - 2 * __style.pageMargins
      wrapMode: Text.WordWrap
      text: qsTr("There are currently no local changes")
      font: __style.t1
      color: __style.forestColor
    }

    Text {
      anchors.horizontalCenter: parent.horizontalCenter
      horizontalAlignment: Text.AlignHCenter
      width: parent.width - 2 * __style.pageMargins
      wrapMode: Text.WordWrap
      text: qsTr("Once you have made some changes, they will appear here.")
      font: __style.p5
      color: __style.nightColor
    }
  }

  // With changes content
  ColumnLayout {
    id: contentLayout

    visible: statusPanel.hasChanges
    height: statusPanel.height - header.height
    width: statusPanel.width - 2*__style.pageMargins
    x: __style.pageMargins
    spacing: 0

    ListView {
      id: statusList

      model: __merginProjectStatusModel
      Layout.fillWidth: true
      Layout.fillHeight: true
      spacing: __style.margin8

      section.property: "section"
      section.criteria: ViewSection.FullString
      section.delegate: Item {
        height: statusPanel.rowHeight/2
        width: ListView.view.width

        Text {
          /* A/ "Pending Changes" or B/ File name */
          anchors.fill: parent
          text: section
          horizontalAlignment: Text.AlignLeft
          verticalAlignment: Text.AlignVCenter
          font: __style.t3
          color: __style.nightColor
        }
      }

      delegate: Item {
        id: delegateItem

        height: fileLabel.visible ? fileLabel.height : row.height
        width: ListView.view.width

        /* A/ Pending changes entries - File name */
        MMProjectStatusItem {
          id: fileLabel

          width: delegateItem.width
          visible: fileStatus !== MerginProjectStatusModel.Changelog
          type: fileStatus
          title: itemText
        }

        /* B/ Per-File changes part */
        ColumnLayout {
          id: row

          visible: fileStatus === MerginProjectStatusModel.Changelog
          width: parent.width

          spacing: __style.margin8

          Text {
            /* Table name within single file */
            id: mainText

            text:itemText
            font: __style.p6
            color: __style.nightColor
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
          }

          MMProjectStatusItem {
            /* Added rows for table */
            id: addedItem

            width: delegateItem.width
            count: inserts
            visible: inserts > 0
            type: MerginProjectStatusModel.Added
          }

          MMProjectStatusItem {
            /* Edited rows for table */
            id: editedItem
            width: delegateItem.width
            count: updates
            visible: updates > 0
            type: MerginProjectStatusModel.Updated
          }

          MMProjectStatusItem {
            /* Deleted rows for table */
            id: deletedItem

            width: delegateItem.width
            count: deletes
            visible: deletes > 0
            type: MerginProjectStatusModel.Deleted
          }
        }
      }
    }
  }
}
