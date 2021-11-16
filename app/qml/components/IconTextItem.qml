/***************************************************************************
 icontextitem.qml
  --------------------------------------
  Date                 : 2019
  Copyright            : (C) 2019 by Viktor Sklencar
  Email                : viktor.sklencar@lutraconsulting.co.uk
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.5
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QgsQuick 0.1 as QgsQuick

Item {
  id: root
  property color fontColor
  property real fontPointSize: root.iconSize * 0.75
  property real iconSize
  property string iconSource
  property string labelText

  ColumnLayout {
    anchors.fill: parent
    spacing: 2 * QgsQuick.Utils.dp

    Item {
      id: iconContainer
      Layout.fillHeight: true
      Layout.preferredHeight: root.height / 2
      Layout.preferredWidth: root.width

      Image {
        id: icon
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        fillMode: Image.PreserveAspectFit
        height: root.iconSize
        source: root.iconSource
        sourceSize.height: height
        sourceSize.width: width
        width: root.iconSize
      }
      ColorOverlay {
        anchors.fill: icon
        color: root.fontColor
        source: icon
      }
    }
    Item {
      id: textContainer
      Layout.fillHeight: true
      Layout.preferredHeight: root.height / 2
      Layout.preferredWidth: root.width

      Text {
        id: text
        color: root.fontColor
        font.pointSize: root.fontPointSize
        horizontalAlignment: Text.AlignHCenter
        maximumLineCount: 3
        text: root.labelText
        width: parent.width
        wrapMode: Text.WordWrap
      }
    }
  }
}
