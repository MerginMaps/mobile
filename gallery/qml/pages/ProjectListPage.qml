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
import "../../app/qml/components"
import "../../app/qml/project"

MMProjectListPage {
  id: pane

  width: parent.width

  headerTitle: "Projects"
  withSearch: true

  onBackClicked: console.log("Back clicked")
}
