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
    id: listview

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

    delegate: Rectangle {
      width: 50
      height: 50
      color: "red"
    }
  }

  Component {
    id: loadingSpinnerComponent

    LoadingSpinner {
      x: parent.width / 2 - width / 2
      running: sourceModel.isLoading
    }
  }
}
