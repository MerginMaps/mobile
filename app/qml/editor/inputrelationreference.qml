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
import QtGraphicalEffects 1.14

import lc 1.0
import ".."

AbstractEditor {
  id: root

  property var parentValue: root.parent.value

  signal editorValueChanged( var newValue, bool isNull )
  signal openLinkedFeature( var linkedFeature )

  onContentClicked: {
    let featurePair = rModel.attributeFromForeignKey( parentValue, FeaturesModel.FeaturePair )

    if ( featurePair == null || !featurePair.valid ) return

     openLinkedFeature( featurePair )
  }

  onRightActionClicked: {
    if ( root.parent.readOnly ) return

    let page = root.parent.formView.push( parentFeaturesPageComponent, { featuresModel: rModel } )
    page.forceActiveFocus()
  }

  onParentValueChanged: title.text = rModel.attributeFromForeignKey( parentValue, FeaturesModel.FeatureTitle ) || ""

  RelationReferenceFeaturesModel {
    id: rModel

    config: root.parent.config
    project: root.parent.activeProject

    onModelReset: title.text = rModel.attributeFromForeignKey( parentValue, FeaturesModel.FeatureTitle ) || ""
  }

  content: Text {
      id: title

      text: root.parent.value

      anchors.fill: parent
      color: customStyle.fields.fontColor
      leftPadding: customStyle.fields.sideMargins
      font.pixelSize: customStyle.fields.fontPixelSize

      horizontalAlignment: Text.AlignLeft
      verticalAlignment: Text.AlignVCenter
    }

  rightAction: Item {
    anchors.fill: parent

    Image {
      id: imgPlus

      y: parent.y + parent.height / 2 - height / 2
      x: parent.x + parent.width - 1.5 * width

      width: parent.width * 0.6
      sourceSize.width: parent.width * 0.6

      source: customStyle.icons.relationsLink
    }

    ColorOverlay {
      source: imgPlus
      color: root.parent.readOnly ? customStyle.toolbutton.backgroundColorInvalid : customStyle.fields.fontColor
      anchors.fill: imgPlus
    }
  }

  Component {
    id: parentFeaturesPageComponent

    FeaturesListPage {
      id: parentFeaturesPage

      pageTitle: qsTr( "Change link" )
      allowSearch: false
      focus: true
      toolbarButtons: ["unlink"]
      toolbarVisible: rModel.allowNull

      onBackButtonClicked: {
        root.parent.formView.pop()
      }

      onSelectionFinished: {
        let fk = rModel.foreignKeyFromAttribute( FeaturesModel.FeatureId, featureIds )
        root.editorValueChanged( fk, false )
        root.parent.formView.pop()
      }

      onUnlinkClicked: {
        root.editorValueChanged( undefined, true )
        root.parent.formView.pop()
      }

      Keys.onReleased: {
        if ( event.key === Qt.Key_Back || event.key === Qt.Key_Escape ) {
          event.accepted = true
          root.parent.formView.pop()
        }
      }
    }
  }
}
