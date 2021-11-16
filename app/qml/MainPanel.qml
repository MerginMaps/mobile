/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.3
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton
import "components"

Item {
  id: mainPanel
  property color gpsIndicatorColor: InputStyle.softRed
  property real itemSize: mainPanel.height * 0.8
  property alias recordButton: recBtnIcon

  focus: true

  signal myLocationClicked
  signal myLocationHold
  signal openBrowseDataClicked
  signal openMapThemesClicked
  signal openProjectClicked
  signal openSettingsClicked
  signal recordClicked
  signal zoomToProject

  Rectangle {
    anchors.fill: parent
    color: InputStyle.clrPanelBackground
    opacity: InputStyle.panelOpacity
  }
  Row {
    id: panelRow
    property real calculatedItemWidth: itemsToShow ? parent.width / itemsToShow : parent.width
    property real itemWidth: mainPanel.height * 1.2
    property int itemsToShow: {
      var possibleItems = Math.min((width / panelRow.itemWidth), children.length) - 1;
      return minItemNumber >= possibleItems ? minItemNumber : possibleItems;
    }
    property int minItemNumber: 4

    anchors.fill: parent
    height: mainPanel.itemSize

    Item {
      id: openProjectsItem
      height: parent.height
      visible: panelRow.itemsToShow > 1
      width: visible ? panelRow.calculatedItemWidth : 0

      MainPanelButton {
        id: openProjectBtn
        imageSource: InputStyle.projectIcon
        text: qsTr("Projects")
        width: mainPanel.itemSize

        onActivated: {
          rootMenu.close();
          mainPanel.openProjectClicked();
        }
      }
    }
    Item {
      id: myLocationItem
      height: parent.height
      visible: panelRow.itemsToShow > 2
      width: visible ? panelRow.calculatedItemWidth : 0

      MainPanelButton {
        id: myLocationBtn
        imageSource: InputStyle.gpsFixedIcon
        imageSource2: InputStyle.gpsNotFixedIcon
        imageSourceCondition: __appSettings.autoCenterMapChecked
        text: qsTr("GPS")
        width: mainPanel.itemSize

        onActivated: {
          rootMenu.close();
          mainPanel.myLocationClicked();
        }
        onActivatedOnHold: {
          rootMenu.close();
          mainPanel.myLocationHold();
        }

        RoundIndicator {
          anchors.right: parent.right
          anchors.top: parent.top
          color: gpsIndicatorColor
          height: width
          width: parent.height / 4
        }
      }
    }
    Item {
      id: recItem
      height: parent.height
      visible: panelRow.itemsToShow > 3
      width: visible ? panelRow.calculatedItemWidth : 0

      MainPanelButton {
        id: recBtn
        text: qsTr("Record")
        width: mainPanel.itemSize

        onActivated: {
          rootMenu.close();
          mainPanel.recordClicked();
        }

        RecordBtn {
          id: recBtnIcon
          anchors.margins: width / 4
          anchors.top: parent.top
          anchors.topMargin: -anchors.margins / 2
          enabled: true
          width: mainPanel.itemSize
        }
      }
    }
    Item {
      id: zoomToProjectItem
      height: parent.height
      visible: panelRow.itemsToShow > 4
      width: visible ? panelRow.calculatedItemWidth : 0

      MainPanelButton {
        id: zoomToProjectBtn
        imageSource: InputStyle.zoomToProjectIcon
        text: qsTr("Zoom to project")
        width: mainPanel.itemSize

        onActivated: {
          rootMenu.close();
          mainPanel.zoomToProject();
        }
      }
    }
    Item {
      id: browseDataItem
      height: parent.height
      visible: panelRow.itemsToShow > 5
      width: visible ? panelRow.calculatedItemWidth : 0

      MainPanelButton {
        id: browseDataBtn
        imageSource: InputStyle.tableIcon
        text: qsTr("Browse data")
        width: mainPanel.itemSize

        onActivated: {
          rootMenu.close();
          mainPanel.openBrowseDataClicked();
        }
      }
    }
    Item {
      id: mapThemesItem
      height: parent.height
      visible: panelRow.itemsToShow > 6
      width: visible ? panelRow.calculatedItemWidth : 0

      MainPanelButton {
        id: mapThemesBtn
        imageSource: InputStyle.mapThemesIcon
        text: qsTr("Map themes")
        width: mainPanel.itemSize

        onActivated: {
          rootMenu.close();
          mainPanel.openMapThemesClicked();
        }
      }
    }

    // Last item
    Item {
      id: settingsItem
      height: parent.height
      visible: panelRow.itemsToShow > 6
      width: visible ? panelRow.calculatedItemWidth : 0

      MainPanelButton {
        id: settingsBtn
        imageSource: InputStyle.settingsIcon
        text: qsTr("Settings")
        width: mainPanel.itemSize

        onActivated: {
          rootMenu.close();
          mainPanel.openSettingsClicked();
        }
      }
    }
    Item {
      height: parent.height
      visible: !settingsItem.visible
      width: panelRow.calculatedItemWidth

      MainPanelButton {
        id: menuBtn
        imageSource: InputStyle.moreMenuIcon
        text: qsTr("More")
        width: mainPanel.itemSize

        onActivated: {
          if (!rootMenu.visible)
            rootMenu.open();
          else
            rootMenu.close();
        }
      }
    }
  }
  Menu {
    id: rootMenu
    closePolicy: Popup.CloseOnReleaseOutsideParent | Popup.CloseOnEscape
    title: qsTr("Menu")
    width: parent.width < 300 * QgsQuick.Utils.dp ? parent.width : 300 * QgsQuick.Utils.dp
    x: parent.width - rootMenu.width
    y: -rootMenu.height

    MenuItem {
      height: visible ? mainPanel.itemSize : 0
      visible: !openProjectsItem.visible
      width: parent.width

      onClicked: {
        openProjectBtn.activated();
        rootMenu.close();
      }

      ExtendedMenuItem {
        contentText: qsTr("Projects")
        height: mainPanel.itemSize
        imageSource: InputStyle.projectIcon
        rowHeight: height
        width: parent.width
      }
    }
    MenuItem {
      height: visible ? mainPanel.itemSize : 0
      visible: !myLocationItem.visible
      width: parent.width

      onClicked: {
        myLocationBtn.activated();
        rootMenu.close();
      }
      onPressAndHold: mainPanel.myLocationHold()

      ExtendedMenuItem {
        contentText: qsTr("GPS")
        height: mainPanel.itemSize
        imageSource: __appSettings.autoCenterMapChecked ? InputStyle.gpsFixedIcon : InputStyle.gpsNotFixedIcon
        rowHeight: height
        width: parent.width

        RoundIndicator {
          anchors.left: parent.left
          anchors.margins: parent.height / 8
          anchors.top: parent.top
          color: gpsIndicatorColor
          height: width
          width: parent.height / 4
        }
      }
    }
    MenuItem {
      height: visible ? mainPanel.itemSize : 0
      visible: !recItem.visible
      width: parent.width

      onClicked: {
        recBtn.activated();
        rootMenu.close();
      }

      ExtendedMenuItem {
        contentText: qsTr("Record")
        height: mainPanel.itemSize
        rowHeight: height
        width: parent.width

        RecordBtn {
          id: recBtnIcon2
          anchors.margins: width / 4
          anchors.topMargin: -anchors.margins / 2
          color: InputStyle.fontColor
          enabled: true
          width: mainPanel.itemSize
        }
      }
    }
    MenuItem {
      height: visible ? mainPanel.itemSize : 0
      visible: !zoomToProjectItem.visible
      width: parent.width

      onClicked: {
        zoomToProjectBtn.activated();
        rootMenu.close();
      }

      ExtendedMenuItem {
        contentText: qsTr("Zoom to project")
        height: mainPanel.itemSize
        imageSource: InputStyle.zoomToProjectIcon
        rowHeight: height
        width: parent.width
      }
    }
    MenuItem {
      height: visible ? mainPanel.itemSize : 0
      visible: !mapThemesItem.visible
      width: parent.width

      onClicked: {
        browseDataBtn.activated();
        rootMenu.close();
      }

      ExtendedMenuItem {
        contentText: qsTr("Browse features")
        height: mainPanel.itemSize
        imageSource: InputStyle.tableIcon
        rowHeight: height
        width: parent.width
      }
    }
    MenuItem {
      height: visible ? mainPanel.itemSize : 0
      visible: !mapThemesItem.visible
      width: parent.width

      onClicked: {
        mapThemesBtn.activated();
        rootMenu.close();
      }

      ExtendedMenuItem {
        contentText: qsTr("Map themes")
        height: mainPanel.itemSize
        imageSource: InputStyle.mapThemesIcon
        rowHeight: height
        width: parent.width
      }
    }
    MenuItem {
      height: visible ? mainPanel.itemSize : 0
      visible: !settingsItem.visible
      width: parent.width

      onClicked: {
        settingsBtn.activated();
        rootMenu.close();
      }

      ExtendedMenuItem {
        anchors.fill: parent
        contentText: qsTr("Settings")
        imageSource: InputStyle.settingsIcon
        rowHeight: parent.height
        width: parent.width
      }
    }
  }

  // Menu shadow
  Rectangle {
    height: rootMenu.height
    layer.enabled: true
    visible: rootMenu.opened
    width: rootMenu.width
    x: rootMenu.x
    y: rootMenu.y

    layer.effect: Shadow {
    }
  }
}
