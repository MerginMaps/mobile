/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0

import QgsQuick 0.1 as QgsQuick

Item {
  id: root

  signal featureClicked( var featureId )

  property bool showAdditionalInfo: false
  property var featuresModel: null

  ListView {
    topMargin: 10 * QgsQuick.Utils.dp
    implicitHeight: parent.height
    implicitWidth: parent.width
    spacing: 8 * QgsQuick.Utils.dp

    model: featuresModel

    delegate: Rectangle {
      id: itemContainer
      width: parent.width
      height: 50 * QgsQuick.Utils.dp

      MouseArea {
        anchors.fill: parent
        onClicked: {
          root.featureClicked( model.FeatureId ? model.FeatureId : index )
        }
      }

      RowLayout {
        id: layout
        anchors.fill: parent

        Item {
          id: iconContainer
          height: itemContainer.height
          width: 60 * QgsQuick.Utils.dp

          Image {
            id: icon
            anchors.centerIn: parent
            anchors.leftMargin: 10 * QgsQuick.Utils.dp
            source: model.IconSource ? model.IconSource : "mIconTableLayer.svg"
            width: 30 * QgsQuick.Utils.dp
            height: width
            sourceSize.width: width
            sourceSize.height: height
            fillMode: Image.PreserveAspectFit
          }
        }

        Item {
          id: textContainer
          height: itemContainer.height
          Layout.fillWidth: true

          Text {
            id: featureTitleText
            text: model.FeatureTitle ? model.FeatureTitle : model.display
            height: textContainer.height/2
            width: textContainer.width
            font.pixelSize: InputStyle.fontPixelSizeNormal
            color: InputStyle.fontColor
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignBottom
            elide: Text.ElideRight
          }

          Text {
            id: descriptionText
            height: textContainer.height/2
            text: {
              if ( showAdditionalInfo )
                return model.Description + ", " + model.FoundPair
              else if ( model.Description )
                return model.Description
              else return "No further information"
            }

            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.top: featureTitleText.bottom
            font.pixelSize: InputStyle.fontPixelSizeSmall
            color: InputStyle.panelBackgroundDark
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignTop
            elide: Text.ElideRight

          }
        }
      }
    }
  }
}
