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

import "../../components"

import mm 1.0 as MM

Control {
  id: root

  property string projectDisplayName
  property string projectId
  property string projectDescription

  property bool projectIsInSync: false
  property real projectSyncProgress: 0.0

  property var projectActionButtons: [] // possible values: upload, changes, sync, download, remove

  property bool projectIsOpened: false
//  property string projectRemoteError

  signal openRequested()
  signal syncRequested()
  signal migrateRequested()
  signal removeRequested()
  signal stopSyncRequested()
  signal showChangesRequested()

  height: implicitHeight

  topPadding: __style.margin20
  rightPadding: __style.margin20
  bottomPadding: __style.margin20
  leftPadding: __style.margin20

  states: [
    State { name: "OnServer" },
    State { name: "UpToDate" }, // downloaded locally, no changes neither on server or locally
    State { name: "Error" },
    State { name: "NeedsSync" }
  ]

  background: Rectangle {
    color: root.projectIsOpened ? __style.forestColor : __style.whiteColor
    radius: __style.radius12
  }

  contentItem: Item {

    implicitHeight: contentColumn.height

    Behavior on implicitHeight {
      NumberAnimation { duration: 150 }
    }

    MouseArea {
      anchors.fill: parent
      onClicked: function( mouse ) {
        mouse.accepted = true
        root.openRequested()
      }
    }

    Column {
      id: contentColumn

      width: parent.width

      spacing: __style.spacing12

      RowLayout {
        // title, desc and menu button
        width: parent.width

        Column {
          Layout.fillWidth: true

          spacing: 0

          MMText {
            width: parent.width

            text: root.projectDisplayName

            font: __style.t3
            color: root.projectIsOpened ? __style.whiteColor : __style.nightColor
          }

          Row {
            width: parent.width

            spacing: __style.margin4

            MMIcon {
              id: projectStatusIcon

              y: parent.height / 2 - height / 2

              source: {
                if ( root.state === "UpToDate" ) return __style.doneCircleIcon
                if ( root.state === "OnServer" ) return __style.cloudIcon
                if ( root.state === "Error" ) return __style.errorCircleIcon
                if ( root.state === "NeedsSync" ) return __style.errorCircleIcon

                return __style.errorCircleIcon
              }

              color: {
                if ( root.state === "UpToDate" ) return __style.positiveColor
                if ( root.state === "OnServer" ) return __style.informativeColor
                if ( root.state === "Error" ) return __style.negativeColor
                if ( root.state === "NeedsSync" ) return __style.warningColor

                return __style.warningColor
              }

              size: __style.icon16
            }

            MMText {
              width: parent.width - projectStatusIcon.width - parent.spacing

              text: root.projectDescription

              font: __style.p6
              color: root.projectIsOpened ? __style.whiteColor : __style.nightColor
            }
          }
        }

        MMRoundButton {
          iconSource: root.state === "OnServer" ? __style.downloadIcon : __style.moreVerticalIcon
          bgndColor: root.projectIsOpened ? __style.whiteColor : __style.lightGreenColor

          onClicked: {
            if ( internal.hasMoreMenu ) {
              moreMenuLoader.active = true
            }
            else {
              root.syncRequested()
            }
          }
        }
      }

      Column {
        // sync component

        width: parent.width
        height: childrenRect.height

        visible: root.projectIsInSync

        spacing: __style.spacing12

        MMProgressBar {
          width: parent.width
          position: root.projectSyncProgress
        }

        Row {
          width: parent.width
          height: childrenRect.height

          spacing: __style.spacing12

          MMText {
            width: parent.width - parent.spacing - stopSyncGroup.width
            text: qsTr( "Synchronising" ) + "..."
            font: __style.p6
            color: root.projectIsOpened ? __style.whiteColor : __style.nightColor
          }

          Item {
            width: stopSyncGroup.width
            height: stopSyncGroup.height

            Row {
              id: stopSyncGroup

              spacing: __style.margin4

              width: childrenRect.width + spacing
              height: childrenRect.height

              MMIcon {
                source: __style.stopIcon
                color: root.projectIsOpened ? __style.whiteColor : __style.forestColor
              }

              MMText {
                text: qsTr( "Stop" )
                font: __style.t4
                color: root.projectIsOpened ? __style.whiteColor : __style.forestColor
              }
            }

            MouseArea {
              anchors.fill: parent
              anchors.margins: -__style.margin12

              onClicked: function( mouse ) {
                mouse.accepted = true
                root.stopSyncRequested()
              }
            }
          }
        }
      }
    }
  }


  Loader {
    id: moreMenuLoader

    active: false
    asynchronous: true

    sourceComponent: MMListDrawer {
      drawerHeader.title: qsTr("More options")

      listModel: ListModel { id: menuModel }

      onClicked: function( type ) { internal.moreMenuItems[type].callback() }

      onClosed: moreMenuLoader.active = false

      function fillMoreMenu() {
        root.projectActionButtons.forEach( ( type ) => {
          let item = internal.moreMenuItems[type]
          item.type = type

          menuModel.append( item )
        } )
      }

      Component.onCompleted: {
        fillMoreMenu()
        open()
      }
    }
  }

  QtObject {
    id: internal

    property bool hasMoreMenu: root.projectActionButtons.length > 1

    // possible options for the "more" menu
    property var moreMenuItems: {
      "download": {
        "name": qsTr("Download"),
        "iconSource": __style.downloadIcon,
        "callback": () => root.syncRequested()
      },
      "sync": {
        "name": qsTr("Synchronize project"),
        "iconSource": __style.syncGreenIcon,
        "callback": () => root.syncRequested()
      },
      "changes": {
        "name": qsTr("Local changes"),
        "iconSource": __style.infoIcon,
        "callback": () => root.showChangesRequested()
      },
      "remove": {
        "name": qsTr("Remove from device"),
        "iconSource": __style.deleteIcon,
        "callback": () => root.removeRequested()
      },
      "upload": {
        "name": qsTr("Upload"),
        "iconSource": __style.uploadIcon,
        "callback": () => root.migrateRequested()
      }
    }
  }

//  Column {
//    id: mainColumn

//    width: parent.width
//    padding: 20 * __dp
//    spacing: 15 * __dp

//    Row {
//      id: row

//      spacing: 6 * __dp

//      Column {
//        id: column

//        Text {
//          width: mainColumn.width - 2 * mainColumn.padding - icon.width - row.spacing
//          height: 26 * __dp

//          text: root.projectDisplayName
//          verticalAlignment: Text.AlignVCenter
//          font: __style.t3
//          color: root.projectIsOpened ? __style.whiteColor : __style.nightColor
//          elide: Text.ElideRight
////          opacity: root.projectIsValid ? 1.0 : 0.4
//        }

//        Row {
//          width: mainColumn.width - 2 * mainColumn.padding - icon.width - row.spacing
//          spacing: 4 * __dp

//          MMIcon {
//            id: errorIcon

//            source: visible ? __style.errorCircleIcon : ""
//            color: __style.negativeColor
////            visible: !root.projectIsValid
//          }

//          Text {
//            width: parent.width - errorIcon.width
//            height: 24 * __dp

//            text: root.projectDescription
//            verticalAlignment: Text.AlignVCenter
//            font: __style.p6
//            elide: Text.ElideRight
//            color: {
////              if(root.projectIsValid) {
//                if(root.projectIsOpened) {
//                  return __style.whiteColor
//                }
//                return __style.nightColor
////              }
////              return __style.grapeColor
//            }
//          }
//        }
//      }

//      MMRoundButton {
//        id: icon

//        bgndColor: __style.lightGreenColor
//        iconSource: __style.moreVerticalIcon

//        onClicked: {
//          root.fillMoreMenu()
//          listDrawer.visible = true
//        }
//      }
//    }

//    Item {
//      height: root.projectIsInSync ? syncColumn.height : 0
//      width: parent.width
//      clip: true

//      Behavior on height {
//        NumberAnimation { duration: 250 }
//      }

//      Column {
//        id: syncColumn

//        spacing: mainColumn.spacing

//        MMProgressBar {
//          position: root.projectSyncProgress
//          width: mainColumn.width - 2 * mainColumn.padding
//        }

//        Row {
//          id: syncRow

//          spacing: 6 * __dp

//          Text {
//            width: mainColumn.width - 2 * mainColumn.padding - stopIcon.width - syncRow.spacing * 2 - stopText.width
//            anchors.verticalCenter: parent.verticalCenter

//            text: qsTr("Synchronising...")
//            font: __style.p6
//            color: root.projectIsOpened ? __style.whiteColor : __style.nightColor
//            elide: Text.ElideRight
//            verticalAlignment: Text.AlignVCenter
//          }

//          MMIcon {
//            id: stopIcon

//            anchors.verticalCenter: parent.verticalCenter

//            source: __style.stopIcon
//            color: root.projectIsOpened ? __style.whiteColor : __style.forestColor

//            MouseArea {
//              anchors.fill: parent
//              onClicked: root.stopSyncRequested()
//            }
//          }

//          Text {
//            id: stopText

//            anchors.verticalCenter: parent.verticalCenter

//            text: qsTr("Stop")
//            font: __style.t4
//            color: root.projectIsOpened ? __style.whiteColor : __style.nightColor
//            verticalAlignment: Text.AlignVCenter

//            MouseArea {
//              anchors.fill: parent
//              onClicked: root.stopSyncRequested()
//            }
//          }
//        }
//      }
//    }
//  }

  // TODO: either drawer or icon

//  MMListDrawer {
//    id: listDrawer

//    drawerHeader.title: qsTr("More options")
//    listModel: ListModel { id: menuModel }

//    onClicked: function(type) {
//      if ( projectRemoteError ) {
//        __notificationModel.addError( qsTr( "Notification: Could not synchronize project, please make sure you are logged in and have sufficient rights." ) )
//        return
//      }
//      switch(type) {
//      case "download": root.syncRequested(); break
//      case "sync": root.syncRequested(); break
//      case "changes": root.showChangesRequested(); break
//      case "remove": root.removeRequested(); break
//      case "upload": root.migrateRequested(); break
//      }
//    }
//  }

//  function getMoreMenuItems() {
//    if ( projectIsMergin && projectIsLocal )
//    {
//      if ( ( projectStatus === MM.ProjectStatus.NeedsSync ) ) {
//        return "sync,changes,remove"
//      }
//      return "changes,remove"
//    }
//    else if ( !projectIsMergin && projectIsLocal ) {
//      return "upload,remove"
//    }
//    return "download"
//  }

//  function fillMoreMenu() {
//    // fill more menu with corresponding items
//    let itemsMap = {
//      "download": {
//        "name": qsTr("Download"),
//        "iconSource": __style.downloadIcon
//      },
//      "sync": {
//        "name": qsTr("Synchronize project"),
//        "iconSource": __style.syncGreenIcon
//      },
//      "changes": {
//        "name": qsTr("Local changes"),
//        "iconSource": __style.infoIcon
//      },
//      "remove": {
//        "name": qsTr("Remove from device"),
//        "iconSource": __style.deleteIcon
//      },
//      "upload": {
//        "name": qsTr("Upload"),
//        "iconSource": __style.uploadIcon
//      }
//    }

//    let items = getMoreMenuItems().split(',')
//    menuModel.clear()
//    items.forEach( function(item) {
//      var json = itemsMap[item]
//      json.type = item
//      menuModel.append( json )
//    } )
//  }
}
