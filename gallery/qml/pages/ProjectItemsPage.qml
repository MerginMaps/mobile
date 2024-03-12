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

import mm 1.0 as MM

import "../../app/qml/project"
import "../../app/qml/project/components"
import "../../app/qml/components"

MMPage {
  id: root

  pageHeader.title: "Project delegates"

  pageBottomMargin: 0
  pageBottomMarginPolicy: MMPage.BottomMarginPolicy.PaintBehindSystemBar

  pageContent: MMScrollView {
    width: parent.width
    height: parent.height

    Column {
      width: parent.width
      height: childrenRect.height

      spacing: __style.spacing12

      MMListSpacer { height: __style.margin30 }

      MMProjectDelegate {
        id: syncingItem

        width: parent.width

        projectIsOpened: true
        projectId: "1"
        projectDisplayName: "Mergin local project"
        projectDescription: "Highlighted"
        projectIsInSync: true
        projectSyncProgress: 1/4

        projectActionButtons: ["download", "sync", "remove"]

        state: "NeedsSync"

        onOpenRequested: console.log("onOpenRequested")
        onStopSyncRequested: projectIsInSync = false
        onShowChangesRequested: console.log("onShowChangesRequested")
        onSyncRequested: {
          projectIsInSync = true
          syncAnimator.start()
        }
        onRemoveRequested: console.log("onRemoveRequested")
        onMigrateRequested: console.log("onMigrateRequested")

        Timer {
          id: syncAnimator

          interval: 300
          repeat: true
          running: true

          onTriggered: {
            if ( syncingItem.projectSyncProgress > 1 ) {
              syncingItem.projectIsInSync = false
              syncingItem.projectSyncProgress = 0
              syncAnimator.running = false
            }

            syncingItem.projectSyncProgress += 1/4
          }
        }
      }

      MMProjectDelegate {
        width: parent.width

        projectIsOpened: false
        projectId: "1"
        projectDisplayName: "Mergin remote project"
        projectDescription: "Highlighted"

        projectActionButtons: ["download"]

        state: "OnServer"

        height: visible ? implicitHeight : 0

        onOpenRequested: console.log("onOpenRequested")
        onStopSyncRequested: projectIsInSync = false
        onShowChangesRequested: console.log("onShowChangesRequested")
        onSyncRequested: projectIsInSync = true
        onRemoveRequested: console.log("onRemoveRequested")
        onMigrateRequested: console.log("onMigrateRequested")
      }

      MMProjectDelegate {
        width: parent.width

        projectIsOpened: false
        projectId: "1"
        projectDisplayName: "Mergin local project Long Long Long Long Long Long Long"
        projectDescription: "Description Description Description Description Description"

        projectActionButtons: ["upload", "changes", "remove"]

        projectIsInSync: true
        projectSyncProgress: 1/3

        state: "UpToDate"

        onOpenRequested: console.log("onOpenRequested")
        onStopSyncRequested: projectIsInSync = false
        onShowChangesRequested: console.log("onShowChangesRequested")
        onSyncRequested: projectIsInSync = true
        onRemoveRequested: console.log("onRemoveRequested")
        onMigrateRequested: console.log("onMigrateRequested")
      }

      MMProjectDelegate {
        width: parent.width

        projectIsOpened: false
        projectId: "2"
        projectDisplayName: "Invalid project"
        projectDescription: "A project error. A project error. A project error."

        projectActionButtons: ["changes", "remove"]

        state: "Error"

        onOpenRequested: console.log("onOpenRequested")
        onStopSyncRequested: console.log("onStopSyncRequested")
        onShowChangesRequested: console.log("onShowChangesRequested")
        onSyncRequested: { console.log("onSyncRequested"); projectIsInSync = true }
        onRemoveRequested: console.log("onRemoveRequested")
        onMigrateRequested: console.log("onMigrateRequested")
      }

      MMListFooterSpacer {}
    }
  }
}
