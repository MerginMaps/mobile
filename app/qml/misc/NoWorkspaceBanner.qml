/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import ".."
import "../components"

Rectangle {
  id: root

  signal createWorkspaceRequested

  color: '#ff4f4f'
  height: InputStyle.rowHeight

  TextWithIcon {
    width: parent.width
    height: InputStyle.rowHeight
    fontColor: 'white'
    bgColor: '#ff4f4f'
    iconColor: 'white'
    linkColor: 'white'
    source: InputStyle.exclamationTriangleIcon
    text: qsTr("%1Workspace is required to use Mergin Maps. Click here to create one%2.")
               .arg("<a href='http://create-workspace'>")
               .arg("</a>")
  }

  MouseArea {
    anchors.fill: parent
    onClicked: root.createWorkspaceRequested()
  }
}
