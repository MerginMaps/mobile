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
import "components"

Item {
  id: root
  property bool allowMultiselect: false
  property var featuresModel: null
  property var selectedFeatures: [] // in/out property, contains list of selected feature ids
  property bool showAdditionalInfo: false

  signal featureClicked(var featureId)
  function toggleFeature(fid) {
    if (selectedFeatures.indexOf(fid) === -1) {
      root.selectedFeatures.push(fid);
    } else {
      root.selectedFeatures = root.selectedFeatures.filter(_id => _id !== fid;);
    }
  }

  ListView {
    implicitHeight: parent.height
    implicitWidth: parent.width
    maximumFlickVelocity: __androidUtils.isAndroid ? InputStyle.scrollVelocityAndroid : maximumFlickVelocity
    model: featuresModel
    spacing: 8 * QgsQuick.Utils.dp
    topMargin: 10 * QgsQuick.Utils.dp

    delegate: Rectangle {
      id: itemContainer
      height: 50 * QgsQuick.Utils.dp
      width: parent.width

      Component.onCompleted: {
        // toggle preselected features
        if (root.selectedFeatures.includes(model.FeatureId))
          checkboxItem.checkState = Qt.Checked;
      }

      MouseArea {
        anchors.fill: parent
        propagateComposedEvents: false

        onClicked: {
          if (allowMultiselect) {
            checkboxItem.toggle();
            root.toggleFeature(model.FeatureId);
          } else
            root.featureClicked(model.FeatureId);
        }
      }
      RowLayout {
        id: layout
        anchors.fill: parent

        Item {
          id: checkboxContainer
          height: itemContainer.height
          visible: allowMultiselect
          width: 40 * QgsQuick.Utils.dp

          LeftCheckBox {
            id: checkboxItem
            anchors.centerIn: parent
            anchors.margins: (parent.height / 4)
            baseColor: InputStyle.panelBackgroundDarker
            height: 40 * QgsQuick.Utils.dp
            width: 40 * QgsQuick.Utils.dp

            onCheckboxClicked: root.toggleFeature(model.FeatureId)
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
            fillMode: Image.PreserveAspectFit
            height: width
            source: __loader.loadIconFromFeature(model.Feature)
            sourceSize.height: height
            sourceSize.width: width
            width: 30 * QgsQuick.Utils.dp
          }
        }
        Item {
          id: textContainer
          Layout.fillWidth: true
          height: itemContainer.height

          Text {
            id: featureTitleText
            color: InputStyle.fontColor
            elide: Text.ElideRight
            font.pixelSize: InputStyle.fontPixelSizeNormal
            height: textContainer.height / 2
            horizontalAlignment: Text.AlignLeft
            text: model.FeatureTitle
            verticalAlignment: Text.AlignBottom
            width: textContainer.width
          }
          Text {
            id: descriptionText
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: featureTitleText.bottom
            color: InputStyle.panelBackgroundDark
            elide: Text.ElideRight
            font.pixelSize: InputStyle.fontPixelSizeSmall
            height: textContainer.height / 2
            horizontalAlignment: Text.AlignLeft
            text: showAdditionalInfo ? model.Description + ", " + model.SearchResult : model.Description
            verticalAlignment: Text.AlignTop
          }
        }
      }
    }
  }
}
