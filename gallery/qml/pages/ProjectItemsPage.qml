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

Page {
  id: pane

  Column {
    width: parent.width
    anchors.top: parent.top
    anchors.topMargin: 20
    anchors.left: parent.left
    anchors.leftMargin: 20
    spacing: 20

    MMProjectDelegate {
      width: 350

      highlight: true
      projectId: "1"
      projectStatus: MM.ProjectStatus.NeedsSync
      projectDisplayName: "Mergin local project"
      projectDescription: "Highlighted"
      projectIsValid: true
      projectIsLocal: true
      projectIsMergin: true
      projectIsPending: true

      onOpenRequested: console.log("onOpenRequested")
      onStopSyncRequested: projectIsPending = false
      onShowChangesRequested: console.log("onShowChangesRequested")
      onSyncRequested: projectIsPending = true
      onRemoveRequested: console.log("onRemoveRequested")
      onMigrateRequested: console.log("onMigrateRequested")
    }

    MMProjectDelegate {
      width: 350

      highlight: false
      projectId: "1"
      projectStatus: MM.ProjectStatus.NoVersion
      projectDisplayName: "Mergin local project"
      projectDescription: "Highlighted"
      projectIsValid: true
      projectIsLocal: true
      projectIsMergin: true

      onOpenRequested: console.log("onOpenRequested")
      onStopSyncRequested: projectIsPending = false
      onShowChangesRequested: console.log("onShowChangesRequested")
      onSyncRequested: projectIsPending = true
      onRemoveRequested: console.log("onRemoveRequested")
      onMigrateRequested: console.log("onMigrateRequested")
    }

    MMProjectDelegate {
      width: 350

      highlight: false
      projectId: "1"
      projectStatus: MM.ProjectStatus.UpToDate
      projectDisplayName: "Mergin local project Long Long Long Long Long Long Long"
      projectDescription: "Description Description Description Description Description"
      projectIsValid: true
      projectIsLocal: true
      projectIsMergin: true

      onOpenRequested: console.log("onOpenRequested")
      onStopSyncRequested: projectIsPending = false
      onShowChangesRequested: console.log("onShowChangesRequested")
      onSyncRequested: projectIsPending = true
      onRemoveRequested: console.log("onRemoveRequested")
      onMigrateRequested: console.log("onMigrateRequested")
    }

    MMProjectDelegate {
      width: 350

      highlight: false
      projectId: "2"
      projectStatus: MM.ProjectStatus.UpToDate
      projectDisplayName: "Invalid project"
      projectDescription: "A project error. A project error. A project error."
      projectIsValid: false
      projectIsLocal: false
      projectIsMergin: false

      onOpenRequested: console.log("onOpenRequested")
      onStopSyncRequested: console.log("onStopSyncRequested")
      onShowChangesRequested: console.log("onShowChangesRequested")
      onSyncRequested: { console.log("onSyncRequested"); projectIsPending = true }
      onRemoveRequested: console.log("onRemoveRequested")
      onMigrateRequested: console.log("onMigrateRequested")
    }

  }
}
