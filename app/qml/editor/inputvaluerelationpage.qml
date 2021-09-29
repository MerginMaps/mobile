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
import "../"

AbstractEditor {
  id: root

  /*required*/ property var parentValue: root.parent.value
  /*required*/ property var fieldConfig: root.parent.config
  /*required*/ property var featureLayerPair: root.parent.featurePair
  /*required*/ property bool isReadOnly: root.parent.readOnly
  /*required*/ property var stackView: root.parent.formView

  property bool allowMultivalue: config["AllowMulti"]

  signal editorValueChanged( var newValue, bool isNull )

  function reload()
  {
    vrModel.pair = root.featureLayerPair
  }

  function setText()
  {
    title.text = vrModel.convertFromQgisType( root.parentValue, FeaturesModel.FeatureTitle ).join( ', ' )
  }

  function pushVrPage()
  {
    let props = {
      featuresModel: vrModel,
      pageTitle: labelAlias,
      allowMultiselect: root.allowMultivalue,
      toolbarVisible: root.allowMultivalue,
      preselectedFeatures: root.allowMultivalue ? vrModel.convertFromQgisType( root.parentValue, FeaturesModel.FeatureId ) : []
    }

    let obj = root.stackView.push( featuresPageComponent, props )
    obj.forceActiveFocus()
  }

  onParentValueChanged: setText()

  onRightActionClicked: pushVrPage()
  onContentClicked: pushVrPage()

  enabled: !isReadOnly

  ValueRelationFeaturesModel {
    id: vrModel

    config: root.fieldConfig
    pair: root.featureLayerPair

    onInvalidate: root.editorValueChanged( "", true )

    onModelReset: setText()
  }

  content: Text {
    id: title

    anchors.fill: parent
    anchors.leftMargin: customStyle.fields.sideMargin
    color: customStyle.fields.fontColor
    font.pointSize: customStyle.fields.fontPointSize
    clip: true

    horizontalAlignment: Text.AlignLeft
    verticalAlignment: Text.AlignVCenter
  }

  rightAction: Item {
    anchors.fill: parent

    Image {
      id: rightArrow

      y: parent.y + parent.height / 2 - height / 2
      x: parent.x + parent.width - 1.5 * width

      source: customStyle.icons.valueRelationMore

      width: parent.width * 0.3
      sourceSize.width: parent.width * 0.3
    }

    ColorOverlay {
      anchors.fill: rightArrow
      source: rightArrow
      color: root.isReadOnly ? customStyle.toolbutton.backgroundColorInvalid : customStyle.fields.fontColor
    }
  }

  Component {
    id: featuresPageComponent

    FeaturesListPage {
      id: featuresPage

      pageTitle: qsTr( "Features" )
      allowSearch: true
      focus: true
      toolbarButtons: ["done"]
      toolbarVisible: false

      onBackButtonClicked: {
        root.stackView.pop()
      }

      onSelectionFinished: {
        if ( root.allowMultivalue )
        {
          let isNull = featureIds.length === 0
          root.editorValueChanged( vrModel.convertToQgisType( featureIds ), isNull )
        }
        else
        {
          root.editorValueChanged( vrModel.convertToKey( featureIds ), false )
        }
        root.stackView.pop()
      }
    }
  }
}
