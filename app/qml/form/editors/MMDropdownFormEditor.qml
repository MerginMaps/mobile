/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import "../../components"
import "../../inputs"

MMBaseInput {
  id: root

  property alias placeholderText: textField.placeholderText
  property alias text: textField.text
  property bool multiSelect: false
  required property ListModel featuresModel
  required property string dropDownTitle
  property var preselectedFeatures: []

  hasFocus: textField.activeFocus

  signal featureClicked( var selectedFeatures )

  content: TextField {
    id: textField

    anchors.fill: parent
    anchors.verticalCenter: parent.verticalCenter

    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
    placeholderTextColor: __style.nightAlphaColor
    font: __style.p5
    hoverEnabled: true
    background: Rectangle {
      color: __style.transparentColor
    }
  }

  rightAction: MMIcon {
    id: eyeButton

    property bool pressed: false

    height: parent.height

    source: __style.arrowDownIcon
    color: root.enabled ? __style.forestColor : __style.mediumGreenColor
  }

  onRightActionClicked: {
    if ( !root.enabled )
      return
    listLoader.active = true
    listLoader.focus = true
  }

  Loader {
    id: listLoader

    asynchronous: true
    active: false
    sourceComponent: listComponent
  }

  Component {
    id: listComponent

    MMComboBoxDrawer {
      focus: true
      model: root.featuresModel
      title: root.dropDownTitle
      multiSelect: root.multiSelect
      preselectedFeatures: root.preselectedFeatures

      Component.onCompleted: open()
      onClosed: listLoader.active = false
      onFeatureClicked: function(selectedFeatures) {
        root.featureClicked( selectedFeatures )
      }
    }
  }
}
