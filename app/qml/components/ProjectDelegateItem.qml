/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.0
import QtQml.Models 2.14

import lc 1.0
import "../"

Rectangle {
  id: root

  // Required properties
  property string projectDisplayName
  property string projectId
  property string projectDescription
  property int projectStatus
  property bool projectIsValid
  property bool projectIsPending
  property real projectSyncProgress
  property bool projectIsLocal
  property bool projectIsMergin
  property string projectRemoteError

  property color primaryColor: InputStyle.clrPanelMain
  property color secondaryColor: InputStyle.fontColor
  property real itemMargin: InputStyle.panelMargin

  property color iconColor: root.highlight ? root.primaryColor : InputStyle.activeButtonColorOrange
  property real iconSize: height * 0.3
  property real borderWidth: 1 * __dp
  property real menuItemHeight: height * 0.8

  property real viewContentY: 0
  property real viewHeight: 0
  property bool highlight: false

  signal openRequested()
  signal syncRequested()
  signal migrateRequested()
  signal removeRequested()
  signal stopSyncRequested()
  signal showChangesRequested()

  function getStatusIcon() {
    if ( projectIsPending )
      return InputStyle.stopIcon

    if ( projectIsLocal && projectIsMergin ) {
      // downloaded mergin projects
      if ( projectStatus === ProjectStatus.OutOfDate ||
           projectStatus === ProjectStatus.Modified ) {
        return InputStyle.syncIcon
      }
      return "" // no icon if this project does not have changes
    }

    if ( projectIsMergin && projectStatus === ProjectStatus.NoVersion )
      return InputStyle.downloadIcon

    if ( projectIsLocal && projectStatus === ProjectStatus.NoVersion )
      return InputStyle.uploadIcon

    return ""
  }

  function getMoreMenuItems() {
    if ( projectIsMergin && projectIsLocal )
    {
      if ( ( projectStatus === ProjectStatus.OutOfDate ||
             projectStatus === ProjectStatus.Modified ) )
        return "sync,changes,remove"

      return "changes,remove"
    }
    else if ( !projectIsMergin && projectIsLocal )
      return "upload,remove"
    else
      return "download"
  }

  function fillMoreMenu() {
    // fill more menu with corresponding items
    let itemsMap = {
      "sync": {
        "name": qsTr("Synchronize project"),
        "iconSource": InputStyle.syncIcon,
        "callback": function() { root.syncRequested() }
      },
      "changes": {
        "name": qsTr("Local changes"),
        "iconSource": InputStyle.infoIcon,
        "callback": function() { root.showChangesRequested() }
      },
      "remove": {
        "name": qsTr("Remove from device"),
        "iconSource": InputStyle.removeIcon,
        "callback": function() { root.removeRequested() }
      },
      "upload": {
        "name": qsTr("Upload to Mergin"),
        "iconSource": InputStyle.uploadIcon,
        "callback": function() { root.migrateRequested() }
      },
      "download": {
        "name": qsTr("Download from Mergin"),
        "iconSource": InputStyle.downloadIcon,
        "callback": function() { root.syncRequested() }
      }
    }

    let items = getMoreMenuItems()
    items = items.split(',')

    // clear previous items
    while( contextMenu.count > 0 )
      contextMenu.takeItem( 0 );

    items.forEach( function(item) { contextMenu.addItem(
                      menuItemComponent.createObject( contextMenu, itemsMap[item] ) )
                  })
    contextMenu.height = items.length * root.menuItemHeight
  }

  color: root.highlight ? InputStyle.panelItemHighlight : root.primaryColor

  MouseArea {
    anchors.fill: parent
    enabled: projectIsValid
    onClicked: openRequested()
  }

  RowLayout {
    id: row

    anchors.fill: parent
    anchors.leftMargin: root.itemMargin
    spacing: 0

    Item {
      id: textContainer

      height: root.height
      Layout.fillWidth: true

      Text {
        id: mainText

        text: __inputUtils.formatProjectName( projectDisplayName )
        height: textContainer.height/2
        width: textContainer.width
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: if (root.highlight) root.primaryColor
               else if (!projectIsValid) InputStyle.panelBackgroundDark
               else root.secondaryColor
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignBottom
        elide: Text.ElideRight
      }

      Text {
        id: secondaryText

        visible: !projectIsPending
        height: textContainer.height/2
        text: projectDescription
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.top: mainText.bottom
        font.pixelSize: InputStyle.fontPixelSizeSmall
        color: root.highlight ? root.primaryColor : InputStyle.panelBackgroundDark
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignTop
        elide: Text.ElideRight
      }

      ProgressBar {
        id: progressBar

        property real itemHeight: InputStyle.rowHeightSmall

        anchors.top: mainText.bottom
        height: itemHeight
        width: secondaryText.width
        value: projectSyncProgress
        visible: projectIsPending

        background: Rectangle {
          implicitWidth: parent.width
          implicitHeight: progressBar.itemHeight
          color: InputStyle.panelBackgroundLight
        }

        contentItem: Item {
          implicitWidth: parent.width
          implicitHeight: progressBar.itemHeight

          Rectangle {
            width: progressBar.visualPosition * parent.width
            height: parent.height
            color: InputStyle.fontColor
          }
        }
      }
    }

    Item {
      id: statusIconContainer

      property string iconSource: getStatusIcon()

      visible: iconSource !== ""
      Layout.preferredWidth: root.height
      height: root.height

      Image {
        id: statusIcon

        anchors.centerIn: parent
        source: statusIconContainer.iconSource
        height: root.iconSize
        width: height
        sourceSize.width: width
        sourceSize.height: height
        fillMode: Image.PreserveAspectFit
      }

      ColorOverlay {
        anchors.fill: statusIcon
        source: statusIcon
        color: root.iconColor
      }

      MouseArea {
        anchors.fill: parent
        onClicked: {
          if ( projectRemoteError ) {
            __inputUtils.showNotification( qsTr( "Could not synchronize project, please make sure you are logged in and have sufficient rights." ) )
            return
          }
          if ( projectIsPending )
            stopSyncRequested()
          else if ( !projectIsMergin )
            migrateRequested()
          else
            syncRequested()
        }
      }
    }

    Item {
      id: moreMenuContainer

      Layout.preferredWidth: root.height
      height: root.height

      Image {
        id: moreMenuIcon

        anchors.centerIn: parent
        source: InputStyle.moreMenuIcon
        height: root.iconSize
        width: height
        sourceSize.width: width
        sourceSize.height: height
        fillMode: Image.PreserveAspectFit
      }

      ColorOverlay {
        anchors.fill: moreMenuIcon
        source: moreMenuIcon
        color: root.iconColor
      }

      MouseArea {
        anchors.fill: parent
        onClicked: {
          fillMoreMenu()
          contextMenu.open()
        }
      }
    }
  }

  Rectangle { // border line
      color: InputStyle.panelBackground2
      width: root.width
      height: root.borderWidth
      anchors.bottom: parent.bottom
  }

  // More Menu
  Menu {
    id: contextMenu

    width: Math.min( root.width, 300 * __dp )
    leftMargin: Math.max( root.width - width, 0 )
    z: 100

    enter: Transition {
      ParallelAnimation {
        NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 100 }
      }
    }
    exit: Transition {
      ParallelAnimation {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0; duration: 100 }
      }
    }

    //! sets y-offset either above or below related item according relative position to end of the list
    onAboutToShow: {
      let itemRelativeY = parent.y - root.viewContentY
      if ( itemRelativeY + contextMenu.height >= root.viewHeight )
        contextMenu.y = -contextMenu.height + parent.height / 3
      else
        contextMenu.y = ( parent.height * 2 ) / 3
    }
  }

  Component {
    id: menuItemComponent

    MenuItem {
      id: menuItem

      property string name: ""
      property var callback: function cb() {} // default callback
      property string iconSource: ""

      height: root.menuItemHeight

      ExtendedMenuItem {
        height: parent.height
        rowHeight: parent.height * 0.8
        width: parent.width
        contentText: menuItem.name
        imageSource: menuItem.iconSource
        overlayImage: true
      }

      onClicked: callback()
    }
  }
}
