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
import QtQuick.Dialogs
import QtQuick.Layouts
import lc 1.0
import "../"
import "."

Item {
  id: root

  property string searchText: ""

  signal workspaceChanged()

  onSearchTextChanged: {
    proxyModel.searchExpression = root.searchText
  }

  ListView {
    id: listView

    anchors.fill: root
    clip: true
    maximumFlickVelocity: __androidUtils.isAndroid ? InputStyle.scrollVelocityAndroid : maximumFlickVelocity

    // Proxy model with source projects model
    model: WorkspacesProxyModel {
      id: proxyModel

      workspacesSourceModel: WorkspacesModel {
        id: model

        merginApi: __merginApi
      }
    }

    spacing: 10

    //delegate: delegateItem
    delegate: Rectangle {
      width: 50
      height: 50
      color: "red"
    }
  }

  Component {
    id: delegateItem

    Rectangle {
      id: itemContainer

      property bool isSelected: __merginApi.userInfo.activeWorkspace === model.display

      width: listView.cellWidth
      height: listView.cellHeight
      anchors.leftMargin: InputStyle.panelMargin
      anchors.rightMargin: InputStyle.panelMargin
      color: item.highlight ? InputStyle.panelItemHighlight : InputStyle.clrPanelMain

      MouseArea {
        anchors.fill: parent
        onClicked: {
          console.log("Change workspace to", model.display)
          root.close()
        }
      }

      ExtendedMenuItem {
          id: item
          panelMargin: InputStyle.panelMargin
          contentText: model.display
          //imageSource: InputStyle.mapThemesIcon
          anchors.rightMargin: panelMargin
          anchors.leftMargin: panelMargin
          highlight: itemContainer.isSelected
          // Do not show border line for selected item and one before selected
          //showBorder: !itemContainer.isSelected && !itemContainer.isOneBeforeSelected
      }
    }
  }

  Component {
    id: loadingSpinnerComponent

    LoadingSpinner {
      x: parent.width / 2 - width / 2
      running: workspacesSourceModel.isLoading
    }
  }
}
