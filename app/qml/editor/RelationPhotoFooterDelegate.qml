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

import lc 1.0
import ".."

Item {
  id: root

  property bool isReadOnly: false

  signal clicked()

  height: parent.height
  width: parent.height + spacingRect.width
  visible: !isReadOnly

  Row {
    height: parent.height
    width: parent.width

    Rectangle { // listview does not add space before footer, add it here
      id: spacingRect

      width: customStyle.group.spacing
      height: width
      color: "transparent"

    }

    Rectangle {
      width: root.width - spacingRect.width
      height: root.height

      border.width: customStyle.relationComponent.photoBorderWidth
      border.color: customStyle.relationComponent.photoBorderColorButton
      color: "transparent"


      Item {
        width: parent.width / 2
        height: parent.height / 2

        anchors.centerIn: parent

        Column {
          anchors.fill: parent

          Item {
            id: iconContainer

            width: parent.width
            height: parent.height - textContainer.height

            Image {
              id: icon

              width: parent.width
              height: parent.height

              sourceSize: Qt.size( width, height )

              source: customStyle.icons.plus
              fillMode: Image.PreserveAspectFit
            }

            ColorOverlay {
              anchors.fill: icon
              source: icon
              color: customStyle.relationComponent.iconColorButton
            }
          }

          Item {
            id: textContainer

            width: parent.width
            height: txt.paintedHeight

            Text {
              id: txt
              text: qsTr( "Add" )

              anchors.centerIn: parent

              font.pixelSize: customStyle.fields.labelPixelSize
              color: customStyle.relationComponent.textColorButton
              clip: true
            }
          }
        }
      }
    }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: root.clicked( model.FeaturePair )
  }
}
