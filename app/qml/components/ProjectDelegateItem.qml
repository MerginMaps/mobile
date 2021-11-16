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
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "../"

Rectangle {
  id: root
  property real borderWidth: 1 * QgsQuick.Utils.dp
  property bool highlight: false
  property color iconColor: root.highlight ? root.primaryColor : InputStyle.activeButtonColorOrange
  property real iconSize: height * 0.3
  property real itemMargin: InputStyle.panelMargin
  property real menuItemHeight: height * 0.8
  property color primaryColor: InputStyle.clrPanelMain
  property string projectDescription

  // Required properties
  property string projectDisplayName
  property string projectId
  property bool projectIsLocal
  property bool projectIsMergin
  property bool projectIsPending
  property bool projectIsValid
  property string projectRemoteError
  property int projectStatus
  property real projectSyncProgress
  property color secondaryColor: InputStyle.fontColor
  property real viewContentY: 0
  property real viewHeight: 0

  color: root.highlight ? InputStyle.panelItemHighlight : root.primaryColor

  function fillMoreMenu() {
    // fill more menu with corresponding items
    let itemsMap = {
      "sync": {
        "name": qsTr("Synchronize project"),
        "iconSource": InputStyle.syncIcon,
        "callback": () =>
          root.syncRequested();
        }
      },
      "changes": {
        "name": qsTr("Local changes"),
        "iconSource": InputStyle.infoIcon,
        "callback": () => root.showChangesRequested();}
      },
      "remove": {
        "name": qsTr("Remove from device"),
        "iconSource": InputStyle.removeIcon,
        "callback": () => root.removeRequested();}
      },
      "upload": {
        "name": qsTr("Upload to Mergin"),
        "iconSource": InputStyle.uploadIcon,
        "callback": () => root.migrateRequested();}
      },
      "download": {
        "name": qsTr("Download from Mergin"),
        "iconSource": InputStyle.downloadIcon,
        "callback": () => root.syncRequested();}
      }
    };
    let items = getMoreMenuItems();
    items = items.split(',');

    // clear previous items
    while (contextMenu.count > 0)
      contextMenu.takeItem(0);
    items.forEach(item => 0
        contextMenu.addItem(menuItemComponent.createObject(contextMenu, itemsMap[item]));
      });
    contextMenu.height = items.length * root.menuItemHeight;
  }
  function getMoreMenuItems() {
    if (projectIsMergin && projectIsLocal) {
      if ((projectStatus === ProjectStatus.OutOfDate || projectStatus === ProjectStatus.Modified))
        return "sync,changes,remove";
      return "changes,remove";
    } else if (!projectIsMergin && projectIsLocal)
      return "upload,remove";
    else
      return "download";
  }
  function getStatusIcon() {
    if (projectIsPending)
      return InputStyle.stopIcon;
    if (projectIsLocal && projectIsMergin) {
      // downloaded mergin projects
      if (projectStatus === ProjectStatus.OutOfDate || projectStatus === ProjectStatus.Modified) {
        return InputStyle.syncIcon;
      }
      return ""; // no icon if this project does not have changes
    }
    if (projectIsMergin && projectStatus === ProjectStatus.NoVersion)
      return InputStyle.downloadIcon;
    if (projectIsLocal && projectStatus === ProjectStatus.NoVersion)
      return InputStyle.uploadIcon;
    return "";
  }
  signal migrateRequested
  signal openRequested
  signal removeRequested
  signal showChangesRequested
  signal stopSyncRequested
  signal syncRequested

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
      Layout.fillWidth: true
      height: root.height

      Text {
        id: mainText
        color: if (root.highlight)
          root.primaryColor
        else if (!projectIsValid)
          InputStyle.panelBackgroundDark
        else
          root.secondaryColor
        elide: Text.ElideRight
        font.pixelSize: InputStyle.fontPixelSizeNormal
        height: textContainer.height / 2
        horizontalAlignment: Text.AlignLeft
        text: __inputUtils.formatProjectName(projectDisplayName)
        verticalAlignment: Text.AlignBottom
        width: textContainer.width
      }
      Text {
        id: secondaryText
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: mainText.bottom
        color: root.highlight ? root.primaryColor : InputStyle.panelBackgroundDark
        elide: Text.ElideRight
        font.pixelSize: InputStyle.fontPixelSizeSmall
        height: textContainer.height / 2
        horizontalAlignment: Text.AlignLeft
        text: projectDescription
        verticalAlignment: Text.AlignTop
        visible: !projectIsPending
      }
      ProgressBar {
        id: progressBar
        property real itemHeight: InputStyle.fontPixelSizeSmall

        anchors.top: mainText.bottom
        height: InputStyle.fontPixelSizeSmall
        value: projectSyncProgress
        visible: projectIsPending
        width: secondaryText.width

        background: Rectangle {
          color: InputStyle.panelBackgroundLight
          implicitHeight: progressBar.itemHeight
          implicitWidth: parent.width
        }
        contentItem: Item {
          implicitHeight: progressBar.itemHeight
          implicitWidth: parent.width

          Rectangle {
            color: InputStyle.fontColor
            height: parent.height
            width: progressBar.visualPosition * parent.width
          }
        }
      }
    }
    Item {
      id: statusIconContainer
      property string iconSource: getStatusIcon()

      Layout.preferredWidth: root.height
      height: root.height
      visible: iconSource !== ""

      Image {
        id: statusIcon
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        height: root.iconSize
        source: statusIconContainer.iconSource
        sourceSize.height: height
        sourceSize.width: width
        width: height
      }
      ColorOverlay {
        anchors.fill: statusIcon
        color: root.iconColor
        source: statusIcon
      }
      MouseArea {
        anchors.fill: parent

        onClicked: {
          if (projectRemoteError) {
            __inputUtils.showNotification(qsTr("Could not synchronize project, please make sure you are logged in and have sufficient rights."));
            return;
          }
          if (projectIsPending)
            stopSyncRequested();
          else if (!projectIsMergin)
            migrateRequested();
          else
            syncRequested();
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
        fillMode: Image.PreserveAspectFit
        height: root.iconSize
        source: InputStyle.moreMenuIcon
        sourceSize.height: height
        sourceSize.width: width
        width: height
      }
      ColorOverlay {
        anchors.fill: moreMenuIcon
        color: root.iconColor
        source: moreMenuIcon
      }
      MouseArea {
        anchors.fill: parent

        onClicked: {
          fillMoreMenu();
          contextMenu.open();
        }
      }
    }
  }
  Rectangle {
    anchors.bottom: parent.bottom // border line
    color: InputStyle.panelBackground2
    height: root.borderWidth
    width: root.width
  }

  // More Menu
  Menu {
    id: contextMenu
    leftMargin: Math.max(root.width - width, 0)
    width: Math.min(root.width, 300 * QgsQuick.Utils.dp)
    z: 100

    //! sets y-offset either above or below related item according relative position to end of the list
    onAboutToShow: {
      let itemRelativeY = parent.y - root.viewContentY;
      if (itemRelativeY + contextMenu.height >= root.viewHeight)
        contextMenu.y = -contextMenu.height + parent.height / 3;
      else
        contextMenu.y = (parent.height * 2) / 3;
    }

    enter: Transition {
      ParallelAnimation {
        NumberAnimation {
          duration: 100
          from: 0
          property: "opacity"
          to: 1.0
        }
      }
    }
    exit: Transition {
      ParallelAnimation {
        NumberAnimation {
          duration: 100
          from: 1.0
          property: "opacity"
          to: 0
        }
      }
    }
  }
  Component {
    id: menuItemComponent
    MenuItem {
      id: menuItem
      property var callback: function cb() {} // default callback
      property string iconSource: ""
      property string name: ""

      height: root.menuItemHeight

      onClicked: callback()

      ExtendedMenuItem {
        contentText: menuItem.name
        height: parent.height
        imageSource: menuItem.iconSource
        overlayImage: true
        rowHeight: parent.height * 0.8
        width: parent.width
      }
    }
  }
}
