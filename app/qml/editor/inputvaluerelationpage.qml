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
  property bool allowMultivalue: config["AllowMulti"]
  /*required*/
  property var featureLayerPair: root.parent.featurePair
  /*required*/
  property var fieldConfig: root.parent.config
  /*required*/
  property bool isReadOnly: root.parent.readOnly

  /*required*/
  property var parentValue: root.parent.value
  /*required*/
  property var stackView: root.parent.formView

  enabled: !isReadOnly

  signal editorValueChanged(var newValue, bool isNull)
  function pushVrPage() {
    let props = {
      "featuresModel": vrModel,
      "pageTitle": labelAlias,
      "allowMultiselect": root.allowMultivalue,
      "toolbarVisible": root.allowMultivalue,
      "preselectedFeatures": root.allowMultivalue ? vrModel.convertFromQgisType(root.parentValue, FeaturesModel.FeatureId) : []
    };
    let obj = root.stackView.push(featuresPageComponent, props);
    obj.forceActiveFocus();
  }
  function reload() {
    if (!root.isReadOnly) {
      vrModel.pair = root.featureLayerPair;
      setText();
    }
  }
  function setText() {
    title.text = vrModel.convertFromQgisType(root.parentValue, FeaturesModel.FeatureTitle).join(', ');
  }

  onContentClicked: pushVrPage()
  onParentValueChanged: {
    vrModel.pair = root.featureLayerPair;
    setText();
  }
  onRightActionClicked: pushVrPage()

  ValueRelationFeaturesModel {
    id: vrModel
    config: root.fieldConfig
    pair: root.featureLayerPair

    onInvalidate: {
      if (!root.isReadOnly) {
        root.editorValueChanged("", true);
      }
    }
  }
  Component {
    id: featuresPageComponent
    FeaturesListPage {
      id: featuresPage
      allowSearch: true
      focus: true
      pageTitle: qsTr("Features")
      toolbarButtons: ["done"]
      toolbarVisible: false

      onBackButtonClicked: {
        root.stackView.pop();
      }
      onSelectionFinished: {
        if (root.allowMultivalue) {
          let isNull = featureIds.length === 0;
          if (!isNull) {
            // We need to convert feature id to string prior to sending it to C++ in order to
            // avoid conversion to scientific notation.
            featureIds = featureIds.map(x => x.toString(););
          }
          root.editorValueChanged(vrModel.convertToQgisType(featureIds), isNull);
        } else {
          // We need to convert feature id to string prior to sending it to C++ in order to
          // avoid conversion to scientific notation.
          featureIds = featureIds.toString();
          root.editorValueChanged(vrModel.convertToKey(featureIds), false);
        }
        root.stackView.pop();
      }
    }
  }

  content: Text {
    id: title
    anchors.fill: parent
    anchors.leftMargin: customStyle.fields.sideMargin
    clip: true
    color: customStyle.fields.fontColor
    font.pointSize: customStyle.fields.fontPointSize
    horizontalAlignment: Text.AlignLeft
    verticalAlignment: Text.AlignVCenter
  }
  rightAction: Item {
    anchors.fill: parent

    Image {
      id: rightArrow
      source: customStyle.icons.valueRelationMore
      sourceSize.width: parent.width * 0.3
      width: parent.width * 0.3
      x: parent.x + parent.width - 1.5 * width
      y: parent.y + parent.height / 2 - height / 2
    }
    ColorOverlay {
      anchors.fill: rightArrow
      color: root.isReadOnly ? customStyle.toolbutton.backgroundColorInvalid : customStyle.fields.fontColor
      source: rightArrow
    }
  }
}
