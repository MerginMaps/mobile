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

  signal workspaceChangeRequested( int workspaceId )
  signal createWorkspaceRequested()

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

    spacing: InputStyle.panelSpacing

    delegate: delegateItem

    footer: DelegateButton {
      id: createWorkspaceButton

      width: listView.width
      btnWidth: listView.width / 2
      height: InputStyle.fieldHeight

      text: qsTr("Create new workspace")
      onClicked: {
        root.createWorkspaceRequested()
      }
    }
  }

  Component {
    id: delegateItem

    Item {
      height: InputStyle.rowHeight
      width: ListView.view.width

      ColumnLayout {
        id: delegateContent

        anchors {
          left: parent.left
          leftMargin: InputStyle.listMarginsSmall
          right: parent.right
          rightMargin: InputStyle.listMarginsSmall
          top: parent.top
        }

        height: parent.height * 0.9

        spacing: 0

        Text {
          Layout.fillWidth: true

          color: InputStyle.fontColor
          font.bold: true
          text: model.display

          font.pixelSize: InputStyle.fontPixelSizeNormal

          elide: Text.ElideMiddle
        }
      }

      Rectangle {
        anchors {
          bottom: parent.bottom
          left: parent.left
          right: parent.right
        }

        height: InputStyle.borderSize
        color: InputStyle.panelBackgroundLight
      }

      MouseArea {
        anchors.fill: parent
        onClicked: root.workspaceChangeRequested( model.whatsThis )
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
