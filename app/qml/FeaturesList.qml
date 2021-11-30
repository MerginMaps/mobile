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

import "./components"

Item {
  id: root

  signal featureClicked( var featureId )

  property bool showAdditionalInfo: false
  property bool allowMultiselect: false
  property var featuresModel: null

  property var selectedFeatures: [] // in/out property, contains list of selected feature ids

  function toggleFeature( fid )
  {
    if ( selectedFeatures.indexOf( fid ) === -1 )
    {
      root.selectedFeatures.push( fid )
    }
    else
    {
      root.selectedFeatures = root.selectedFeatures.filter( function (_id) { return _id !== fid } )
    }
  }

  ListView {
    topMargin: 10 * __dp
    implicitHeight: parent.height
    implicitWidth: parent.width
    spacing: 8 * __dp
    maximumFlickVelocity: __androidUtils.isAndroid ? InputStyle.scrollVelocityAndroid : maximumFlickVelocity

    model: featuresModel

    delegate: Rectangle {
      id: itemContainer
      width: parent.width
      height: 50 * __dp

      MouseArea {
        anchors.fill: parent
        propagateComposedEvents: false
        onClicked: {
          if ( allowMultiselect ) {
            checkboxItem.toggle()
            root.toggleFeature( model.FeatureId )
          }
          else root.featureClicked( model.FeatureId )
        }
      }

      Component.onCompleted: { // toggle preselected features
        if ( root.selectedFeatures.includes( model.FeatureId ) )
          checkboxItem.checkState = Qt.Checked
      }

      RowLayout {
        id: layout
        anchors.fill: parent

        Item {
          id: checkboxContainer
          visible: allowMultiselect
          height: itemContainer.height
          width: 40 * __dp

          LeftCheckBox {
            id: checkboxItem
            anchors.margins: (parent.height / 4)
            anchors.centerIn: parent
            baseColor: InputStyle.panelBackgroundDarker
            height: 40 * __dp
            width: 40 * __dp

            onCheckboxClicked: root.toggleFeature( model.FeatureId )
          }
        }

        Item {
          id: iconContainer
          height: itemContainer.height
          width: checkboxContainer.visible ? 30 * __dp : 60 * __dp

          Image {
            id: icon
            anchors.centerIn: parent
            anchors.leftMargin: 10 * __dp
            source: __loader.loadIconFromFeature( model.Feature )
            width: 30 * __dp
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
            text: showAdditionalInfo ? model.Description + ", " + model.SearchResult : model.Description
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
