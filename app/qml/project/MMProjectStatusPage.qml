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

import mm 1.0 as MM

import "./components" as MMProjectComponents
import "../components" as MMComponents

MMComponents.MMPage {
  id: statusPanel

  property real rowHeight: __style.row49
  property bool hasChanges: true

  signal back()

  pageHeader.title: statusPanel.hasChanges ? qsTr("Your local changes") : qsTr("Project Status")
  onBackClicked: statusPanel.back()

  pageContent: Item {
    width: parent.width
    height: parent.height

    // No changes content
    MMComponents.MMMessage {
      id: noChangesContent

      visible: !statusPanel.hasChanges
      anchors.verticalCenter: parent.verticalCenter
      width: parent.width

      image: __style.syncImage
      title: qsTr("There are currently no local changes")
      description: qsTr("Once you have made some changes, they will appear here.")
    }

    MMComponents.MMListSpacer { id: spacer; height: __style.spacing40 }

    // With changes content
    MMComponents.MMListView {
      id: statusList

      anchors.top: spacer.bottom
      anchors.bottom: parent.bottom
      width: parent.width
      visible: statusPanel.hasChanges

      model: __merginProjectStatusModel

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
        MMProjectComponents.MMProjectStatusItem {
          id: fileLabel

          width: delegateItem.width
          visible: fileStatus !== MM.MerginProjectStatusModel.Changelog
          type: fileStatus
          title: itemText
        }

        /* B/ Per-File changes part */
        ColumnLayout {
          id: row

          visible: fileStatus === MM.MerginProjectStatusModel.Changelog
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

          MMProjectComponents.MMProjectStatusItem {
            /* Added rows for table */
            id: addedItem

            width: delegateItem.width
            count: inserts
            visible: inserts > 0
            type: MM.MerginProjectStatusModel.Added
          }

          MMProjectComponents.MMProjectStatusItem {
            /* Edited rows for table */
            id: editedItem
            width: delegateItem.width
            count: updates
            visible: updates > 0
            type: MM.MerginProjectStatusModel.Updated
          }

          MMProjectComponents.MMProjectStatusItem {
            /* Deleted rows for table */
            id: deletedItem

            width: delegateItem.width
            count: deletes
            visible: deletes > 0
            type: MM.MerginProjectStatusModel.Deleted
          }
        }
      }
    }
  }
}
