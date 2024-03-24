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

import "../components"

MMListDrawer {
  id: root

  drawerHeader.title: qsTr("Map Themes")
  drawerHeader.titleFont: __style.t2

  emptyStateDelegate: MMMessage {
    image: __style.noMapThemesImage
    title: qsTr("There are currently no map themes")
    description: qsTr("Learn more about <a href='%1' style='color: %2;'>how to setup themes</a>.")
    .arg(__inputHelp.howToSetupThemesLink)
    .arg(__style.forestColor)
  }

  list.model: MM.MapThemesModel {
      id: mapThemesModel
      qgsProject: __activeProject.qgsProject
  }

  list.delegate: MMListDelegate {
    text: model.display

    rightContent: MMIcon {
      source: __style.doneCircleIcon
      visible: __activeProject.mapTheme === model.display
    }

    onClicked: {
      __activeProject.mapTheme = model.display
      root.close()
    }
  }

  Connections {
    target: __activeProject

    function onProjectWillBeReloaded() {
      mapThemesModel.reset()
    }

    function onProjectReloaded( qgsProject ) {
      mapThemesModel.qgsProject = __activeProject.qgsProject
    }
  }
}
