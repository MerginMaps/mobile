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

    anchors.fill: parent
    clip: true
    maximumFlickVelocity: __androidUtils.isAndroid ? InputStyle.scrollVelocityAndroid : maximumFlickVelocity

    // Proxy model with source projects model
    model: WorkspacesProxyModel {
      id: proxyModel

      sourceModel: WorkspacesModel {
        id: model

        merginApi: __merginApi
      }
    }
  }
}
