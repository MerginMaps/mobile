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

import mm 1.0 as MM
import MMInput

import "../components"

MMListDrawer {
  id: root

  drawerHeader.title: qsTr("Map Themes")
  drawerHeader.titleFont: __style.t2

  emptyStateDelegate: MMMessage {
    image: __style.noMapThemesImage
    title: qsTr("There are currently no map themes")
    description: __inputUtils.htmlLink(
                   qsTr("Learn more about %1how to setup themes%2."),
                   __style.forestColor,
                   __inputHelp.howToSetupThemesLink
                 )
  }

  list.model: MM.MapThemesModel {
      id: mapThemesModel
      qgsProject: ActiveProject.qgsProject
  }

  list.delegate: MMListDelegate {
    text: model.display

    rightContent: MMIcon {
      source: __style.doneCircleIcon
      visible: ActiveProject.mapTheme === model.display
    }

    onClicked: {
      ActiveProject.mapTheme = model.display
      root.close()
    }
  }

  Connections {
    target: ActiveProject

    function onProjectWillBeReloaded() {
      mapThemesModel.reset()
    }

    function onProjectReloaded( qgsProject ) {
      mapThemesModel.qgsProject = ActiveProject.qgsProject
    }
  }
}
