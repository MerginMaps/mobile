/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.0
import ".."
import "../components"

Rectangle {
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
    text: qsTr("Your attention is required. Please visit the %1Mergin dashboard%2.")
               .arg("<a href='" + __inputHelp.merginDashboardLink + "'>")
               .arg("</a>")
  }

  MouseArea {
    anchors.fill: parent
    onClicked: Qt.openUrlExternally( __inputHelp.merginDashboardLink )
  }
}
