/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQml.Models 2.14
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "../"

Item {
  id: root
  property bool isReadOnly: false

  height: parent.height
  visible: !isReadOnly
  width: parent.height + spacingRect.width

  signal clicked

  Row {
    height: parent.height
    width: parent.width

    Rectangle {
      // listview does not add space before footer, add it here
      id: spacingRect
      color: "transparent"
      height: width
      width: customStyle.group.spacing
    }
    Rectangle {
      border.color: customStyle.relationComponent.photoBorderColorButton
      border.width: customStyle.relationComponent.photoBorderWidth
      color: "transparent"
      height: root.height
      width: root.width - spacingRect.width

      Item {
        anchors.centerIn: parent
        height: parent.height / 2
        width: parent.width / 2

        Column {
          anchors.fill: parent

          Item {
            id: iconContainer
            height: parent.height - textContainer.height
            width: parent.width

            Image {
              id: icon
              fillMode: Image.PreserveAspectFit
              height: parent.height
              source: customStyle.icons.plus
              sourceSize: Qt.size(width, height)
              width: parent.width
            }
            ColorOverlay {
              anchors.fill: icon
              color: customStyle.relationComponent.iconColorButton
              source: icon
            }
          }
          Item {
            id: textContainer
            height: txt.paintedHeight
            width: parent.width

            Text {
              id: txt
              anchors.centerIn: parent
              clip: true
              color: customStyle.relationComponent.textColorButton
              font.pointSize: customStyle.fields.labelPointSize
              text: qsTr("Add")
            }
          }
        }
      }
    }
  }
  MouseArea {
    anchors.fill: parent

    onClicked: root.clicked(model.FeaturePair)
  }
}
