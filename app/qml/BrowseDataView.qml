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



Item {
  id: root

  signal featureClicked( var featureId )
  signal featureToggled( var featureId, var toggleState )

  property bool showAdditionalInfo: false
  property bool allowMultiselect: false
  property var featuresModel: null
  property var preSelectedIds: []

  ListView {
    topMargin: 10 * QgsQuick.Utils.dp
    implicitHeight: parent.height
    implicitWidth: parent.width
    spacing: 8 * QgsQuick.Utils.dp
    maximumFlickVelocity: __androidUtils.isAndroid ? InputStyle.scrollVelocityAndroid : maximumFlickVelocity

    model: featuresModel

    delegate: Rectangle {
      id: itemContainer
      width: parent.width
      height: 50 * QgsQuick.Utils.dp

      MouseArea {
        anchors.fill: parent
        propagateComposedEvents: false
        onClicked: {

          if ( allowMultiselect ) {
            checkboxItem.toggle()
            root.featureToggled( model.FeatureId, checkboxItem.checkState )
          }
          else root.featureClicked( model.FeatureId )

        }
      }

      Component.onCompleted: { // mark all preselected features
        if ( Array.isArray( preSelectedIds ) ) {
          if ( preSelectedIds.includes( model.FeatureId ) ) {
            checkboxItem.checkState = Qt.Checked
          }
        }
      }

      RowLayout {
        id: layout
        anchors.fill: parent

        Item {
          id: checkboxContainer
          visible: allowMultiselect
          height: itemContainer.height
          width: 40 * QgsQuick.Utils.dp

          LeftCheckBox {
            id: checkboxItem
            anchors.margins: (parent.height / 4)
            anchors.centerIn: parent
            baseColor: InputStyle.panelBackgroundDarker
            height: 40 * QgsQuick.Utils.dp
            width: 40 * QgsQuick.Utils.dp

            onCheckboxClicked: root.featureToggled( model.FeatureId, buttonState )
          }
        }

        Item {
          id: iconContainer
          height: itemContainer.height
          width: checkboxContainer.visible ? 30 * QgsQuick.Utils.dp : 60 * QgsQuick.Utils.dp

          Image {
            id: icon
            anchors.centerIn: parent
            anchors.leftMargin: 10 * QgsQuick.Utils.dp
            source: __loader.loadIconFromFeature( model.Feature )
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
            text: model.FeatureTitle
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
            text: showAdditionalInfo ? model.Description + ", " + model.FoundPair : model.Description
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
