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
import lc 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton
import "components"

Drawer {
  id: mapThemePanel
  dragMargin: 0 // prevents opening the drawer by dragging.
  interactive: false
  modal: true
  visible: false

  Item {
    focus: true

    Keys.onReleased: {
      if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
        mapThemePanel.close();
      }
    }
  }
  PanelHeader {
    id: header
    color: InputStyle.panelBackgroundLight
    height: InputStyle.rowHeightHeader
    layer.enabled: true
    rowHeight: InputStyle.rowHeightHeader
    titleText: qsTr("Map Themes")
    width: parent.width
    withBackButton: true

    onBack: mapThemePanel.close()

    layer.effect: Shadow {
    }
  }
  ListView {
    id: listView
    property int borderWidth: 1
    property int cellHeight: InputStyle.rowHeight
    property int cellWidth: width

    clip: true
    delegate: delegateItem
    height: mapThemePanel.height - header.height
    implicitHeight: contentHeight
    implicitWidth: parent.width
    model: __mapThemesModel
    width: parent.width
    y: header.height

    TextHyperlink {
      anchors.fill: parent
      text: qsTr("Project has no themes defined. See %1how to setup themes%2.").arg("<a href='" + __inputHelp.howToSetupThemesLink + "'>").arg("</a>")
      visible: parent.count == 0
    }
  }
  Component {
    id: delegateItem
    Rectangle {
      id: itemContainer
      anchors.leftMargin: InputStyle.panelMargin
      anchors.rightMargin: InputStyle.panelMargin
      color: item.highlight ? InputStyle.panelItemHighlight : InputStyle.clrPanelMain
      height: listView.cellHeight
      width: listView.cellWidth

      MouseArea {
        anchors.fill: parent

        onClicked: {
          __loader.setActiveMapTheme(index);
          mapThemePanel.close();
        }
      }
      ExtendedMenuItem {
        id: item
        anchors.leftMargin: panelMargin
        anchors.rightMargin: panelMargin
        contentText: name
        highlight: __mapThemesModel.activeThemeIndex === index
        imageSource: InputStyle.mapThemesIcon
        panelMargin: InputStyle.panelMargin
        showBorder: __mapThemesModel.activeThemeIndex - 1 !== index && __mapThemesModel.activeThemeIndex !== index
      }
    }
  }

  background: Rectangle {
    color: InputStyle.clrPanelMain
  }
}
