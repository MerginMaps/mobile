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

import "."
import "./form" as Forms
import QgsQuick 0.1 as QgsQuick
import lc 1.0

Item {
  id: root

  /*
   * FeaturePanel component is responsible for entire feature form, both preview and fullscreen form
   */

  property var project
  property real previewHeight

  property int activeFormIndex: 0

  function openForm( pair, formState, panelState ) {
    let props = {
      featureLayerPair: pair,
      startingState: panelState,
      formState: formState
    }

    root.visible = true

    let latest = formsStack.push( formComponent, props)
  }

  function setFeaturePair( pair ) {
    if ( root.activeFormIndex >= 0 && root.activeFormIndex < formsStack.depth ) {
      let form = formsStack.get( activeFormIndex )
      form.featureLayerPair = pair
    }
  }

  function getFeaturePair() {
    if ( root.activeFormIndex >= 0 && root.activeFormIndex < formsStack.depth ) {
      let form = formsStack.get( activeFormIndex )
      return form.featureLayerPair
    }
  }

  function reload() {
    // TODO: REMOVE EVERYTHING ~ CLEAR STACK
    formsStack.clear()
//    attributeController.reset()
//    featureForm.reset()
//    rememberAttributesController.reset()
//    attributePreviewController.reset()
  }

//  function show_panel(feature, formState, panelState) {
//      featurePanel.feature = feature
//      attributePreviewController.featureLayerPair = feature
//      featurePanel.formState = formState
//      featurePanel.visible = true
//      featurePanel.isReadOnly = feature.layer.readOnly
//      backHandler.focus = true
//      stateManager.state = panelState
//  }

  function isNewFeature() {
//    return attributeController.isNewFeature()
    if ( root.activeFormIndex >= 0 && root.activeFormIndex < formsStack.depth ) {
      let form = formsStack.get( activeFormIndex )
//      return form.isNewFeature()
      return true
    }
    return null
  }

  function save() {
    if ( root.activeFormIndex >= 0 && root.activeFormIndex < formsStack.depth ) {
      let form = formsStack.get( activeFormIndex )
//      form.save()
    }
  }

  StackView {
    id: formsStack

    anchors.fill: parent
  }

  Component {
    id: formComponent

    Forms.FormWrapper {
      id: wrapper

      project: root.project
      previewHeight: root.previewHeight
    }
  }
}

