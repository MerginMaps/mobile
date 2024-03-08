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

import "../../components"

import mm 1.0 as MM

Rectangle {
  id: root

  property string projectDisplayName
  property string projectId
  property string projectDescription
  property int projectStatus
  property bool projectIsValid
  property bool projectIsLocal
  property bool projectIsMergin
  property bool projectIsPending: false
  property real projectSyncProgress: 0.0
  property bool highlight: false
  property string projectRemoteError

  signal openRequested()
  signal syncRequested()
  signal migrateRequested()
  signal removeRequested()
  signal stopSyncRequested()
  signal showChangesRequested()

  color: root.highlight ? __style.forestColor : __style.whiteColor
  radius: __style.radius12
  height: visible ? mainColumn.height : 0

  MouseArea {
    anchors.fill: parent
    enabled: root.projectIsValid
    onClicked: openRequested()
  }

  Column {
    id: mainColumn

    width: parent.width
    padding: 20 * __dp
    spacing: 15 * __dp

    Row {
      id: row

      spacing: 6 * __dp

      Column {
        id: column

        Text {
          width: mainColumn.width - 2 * mainColumn.padding - icon.width - row.spacing
          height: 26 * __dp

          text: root.projectDisplayName
          verticalAlignment: Text.AlignVCenter
          font: __style.t3
          color: root.highlight ? __style.whiteColor : __style.nightColor
          elide: Text.ElideRight
          opacity: root.projectIsValid ? 1.0 : 0.4
        }

        Row {
          width: mainColumn.width - 2 * mainColumn.padding - icon.width - row.spacing
          spacing: 4 * __dp

          MMIcon {
            id: errorIcon

            source: visible ? __style.errorCircleIcon : ""
            color: __style.negativeColor
            visible: !root.projectIsValid
          }

          Text {
            width: parent.width - errorIcon.width
            height: 24 * __dp

            text: root.projectDescription
            verticalAlignment: Text.AlignVCenter
            font: __style.p6
            elide: Text.ElideRight
            color: {
              if(root.projectIsValid) {
                if(root.highlight) {
                  return __style.whiteColor
                }
                return __style.nightColor
              }
              return __style.grapeColor
            }
          }
        }
      }

      MMRoundButton {
        id: icon

        bgndColor: __style.lightGreenColor
        iconSource: __style.moreVerticalIcon

        onClicked: {
          root.fillMoreMenu()
          listDrawer.visible = true
        }
      }
    }

    Item {
      height: root.projectIsPending ? syncColumn.height : 0
      width: parent.width
      clip: true

      Behavior on height {
        NumberAnimation { duration: 250 }
      }

      Column {
        id: syncColumn

        spacing: mainColumn.spacing

        MMProgressBar {
          position: root.projectSyncProgress
          width: mainColumn.width - 2 * mainColumn.padding
        }

        Row {
          id: syncRow

          spacing: 6 * __dp

          Text {
            width: mainColumn.width - 2 * mainColumn.padding - stopIcon.width - syncRow.spacing * 2 - stopText.width
            anchors.verticalCenter: parent.verticalCenter

            text: qsTr("Synchronising...")
            font: __style.p6
            color: root.highlight ? __style.whiteColor : __style.nightColor
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
          }

          MMIcon {
            id: stopIcon

            anchors.verticalCenter: parent.verticalCenter

            source: __style.stopIcon
            color: root.highlight ? __style.whiteColor : __style.forestColor

            MouseArea {
              anchors.fill: parent
              onClicked: root.stopSyncRequested()
            }
          }

          Text {
            id: stopText

            anchors.verticalCenter: parent.verticalCenter

            text: qsTr("Stop")
            font: __style.t4
            color: root.highlight ? __style.whiteColor : __style.nightColor
            verticalAlignment: Text.AlignVCenter

            MouseArea {
              anchors.fill: parent
              onClicked: root.stopSyncRequested()
            }
          }
        }
      }
    }
  }

  MMListDrawer {
    id: listDrawer

    drawerHeader.title: qsTr("More options")
    listModel: ListModel { id: menuModel }

    onClicked: function(type) {
      if ( projectRemoteError ) {
        __notificationModel.addError( qsTr( "Notification: Could not synchronize project, please make sure you are logged in and have sufficient rights." ) )
        return
      }
      switch(type) {
      case "download": root.syncRequested(); break
      case "sync": root.syncRequested(); break
      case "changes": root.showChangesRequested(); break
      case "remove": root.removeRequested(); break
      case "upload": root.migrateRequested(); break
      }
    }
  }

  function getMoreMenuItems() {
    if ( projectIsMergin && projectIsLocal )
    {
      if ( ( projectStatus === 2 /*ProjectStatus.NeedsSync*/ ) ) { // uncomment when using this component
        return "sync,changes,remove"
      }
      return "changes,remove"
    }
    else if ( !projectIsMergin && projectIsLocal ) {
      return "upload,remove"
    }
    return "download"
  }

  function fillMoreMenu() {
    // fill more menu with corresponding items
    let itemsMap = {
      "download": {
        "name": qsTr("Download"),
        "iconSource": __style.downloadIcon
      },
      "sync": {
        "name": qsTr("Synchronize project"),
        "iconSource": __style.syncGreenIcon
      },
      "changes": {
        "name": qsTr("Local changes"),
        "iconSource": __style.infoIcon
      },
      "remove": {
        "name": qsTr("Remove from device"),
        "iconSource": __style.deleteIcon
      },
      "upload": {
        "name": qsTr("Upload"),
        "iconSource": __style.uploadIcon
      }
    }

    let items = getMoreMenuItems().split(',')
    menuModel.clear()
    items.forEach( function(item) {
      var json = itemsMap[item]
      json.type = item
      menuModel.append( json )
    } )
  }
}
