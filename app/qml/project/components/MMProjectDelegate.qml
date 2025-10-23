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

  signal openRequested()
  signal syncRequested()
  signal migrateRequested()
  signal removeRequested()
  signal stopSyncRequested()
  signal showChangesRequested()

  height: implicitHeight
  width: ListView.view.width - ListView.view.scrollSpace

  topPadding: __style.margin20
  rightPadding: __style.margin20
  leftPadding: __style.margin20
  // The last item in a section ('currently open' vs 'downloaded projects' in Home tab) gets an extra
  // __style.margin28 padding, then the inset is also grown to keep the background at the correct size
  bottomPadding: ListView.section !== ListView.nextSection ? __style.margin48: __style.margin20
  bottomInset: ListView.section !== ListView.nextSection ? + __style.margin28: topInset

  states: [
    State { name: "OnServer" },
    State { name: "UpToDate" }, // downloaded locally, no changes neither on server or locally
    State { name: "Error" },
    State { name: "NeedsSync" }
  ]

  background: Rectangle {
    color: root.projectIsOpened ? __style.forestColor : __style.polarColor
    radius: __style.radius12
    MouseArea {
      anchors.fill: parent
      onClicked: function( mouse ) {
        mouse.accepted = true
        root.openRequested()
      }
    }
  }

  contentItem: Item {

    implicitHeight: contentColumn.height

    Behavior on implicitHeight {
      NumberAnimation { duration: 150 }
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
            color: {
              if ( root.projectIsOpened ) return __style.polarColor
              if ( root.state === "Error" ) return __style.darkGreyColor
              return __style.nightColor
            }

            elide: Text.ElideLeft
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

              text: projectIsInSync ? qsTr( "Synchronising project changes" ) : root.projectDescription

              font: __style.p6
              color: root.projectIsOpened ? __style.polarColor : __style.nightColor
            }
          }
        }

        MMRoundButton {
          iconSource: root.state === "OnServer" ? __style.downloadIcon : __style.moreVerticalIcon
          bgndColor: root.projectIsOpened ? __style.polarColor : __style.lightGreenColor

          visible: !(root.state === "OnServer" && root.projectIsInSync)

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
            text: qsTr( "Please don't close the app." )
            font: __style.p6
            color: root.projectIsOpened ? __style.polarColor : __style.nightColor
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
                color: root.projectIsOpened ? __style.polarColor : __style.forestColor
              }

              MMText {
                text: qsTr( "Stop" )
                font: __style.t4
                color: root.projectIsOpened ? __style.polarColor : __style.forestColor
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
      id: moreMenu

      drawerHeader.title: qsTr("More options")

      list.model: ListModel { id: menuModel }

      list.delegate: MMListDelegate {
        text: model.name

        leftContent: MMIcon {
          source: model.iconSource
        }

        onClicked: {
          let type = model.type
          internal.moreMenuItems[type].callback()
          moreMenu.close()
        }
      }

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
        "name": qsTr("Synchronise project"),
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
}
