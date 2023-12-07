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

import "../components"

Rectangle {
  id: root

  required property string projectDisplayName
  required property string projectId
  required property string projectDescription
  required property int projectStatus
  required property bool projectIsValid
  required property bool projectIsLocal
  required property bool projectIsMergin
  property bool projectIsPending: false
  property real projectSyncProgress: 0.0
  property bool highlight: false

  signal openRequested()
  signal syncRequested()
  signal migrateRequested()
  signal removeRequested()
  signal stopSyncRequested()
  signal showChangesRequested()

  color: root.highlight ? __style.forestColor : __style.whiteColor
  radius: 12 * __dp
  height: mainColumn.height

  MouseArea {
    anchors.fill: parent
    enabled: root.projectIsValid
    onClicked: openRequested()
  }

  Column {
    id: mainColumn

    width: parent.width
    padding: 20 * __dp
    spacing: 12 * __dp

    Row {
      id: row

      spacing: 6 * __dp

      Column {
        id: column

        spacing: 6 * __dp

        Text {
          width: mainColumn.width - 2 * mainColumn.padding - icon.width - row.spacing

          text: root.projectDisplayName
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

            source: visible ? __style.errorIcon : ""
            color: __style.negativeColor
            visible: !root.projectIsValid
          }

          Text {
            width: parent.width - errorIcon.width

            text: root.projectDescription
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

      Image {
        id: icon

        height: column.height
        width: height

        source: __style.projectButtonMoreIcon
        fillMode: Image.PreserveAspectFit

        MouseArea {
          anchors.fill: parent
          onClicked: {
            root.fillMoreMenu()
            listDrawer.visible = true
          }
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

    title: qsTr("More options")
    model: ListModel {
      id: menuModel
    }

    onClicked: function(type) {
      console.log(type)
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
        "name": qsTr("Download from Mergin"),
        "iconSource": __style.downloadIcon
      },
      "sync": {
        "name": qsTr("Synchronize project"),
        "iconSource": __style.syncIcon
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
        "name": qsTr("Upload to Mergin"),
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
