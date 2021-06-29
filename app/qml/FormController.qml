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

  signal editGeometry()
  signal closed()

  function openForm( pair, formState, panelState ) {
    if ( formsStack.depth === 0 )
    {
      let props = {
        featureLayerPair: pair,
        formState: formState,
        panelState: panelState
      }

      var latest = formsStack.push( formComponent, props)
    }
    else
    {
      latest = formsStack.get( 0 )
      latest.featureLayerPair = pair
      latest.formState = formState
      latest.panelState = panelState
    }
    root.activeFormIndex = latest.StackView.index
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
    formsStack.clear() // removes all objects due to Qt parent system
  }

  function isNewFeature() {
    if ( root.activeFormIndex >= 0 && root.activeFormIndex < formsStack.depth ) {
      let form = formsStack.get( activeFormIndex )
      return form.isNewFeature()
    }
  }

  function updateFeatureGeometry() {
    if ( root.activeFormIndex >= 0 && root.activeFormIndex < formsStack.depth ) {
      let form = formsStack.get( activeFormIndex )
      form.updateFeatureGeometry( activeFormIndex )
    }
  }

  function geometryEditingFinished( formState ) {
    if ( root.activeFormIndex >= 0 && root.activeFormIndex < formsStack.depth ) {
      let form = formsStack.get( activeFormIndex )
      form.formState = formState
      form.panelState = "form"
    }
  }

  function geometryEditingStarted( formIndex ) {
    activeFormIndex = formIndex
    hide()
    editGeometry()
  }

  function closeDrawer() {
    if ( root.activeFormIndex >= 0 && root.activeFormIndex < formsStack.depth ) {
      let form = formsStack.get( activeFormIndex )
      form.closeDrawer()
    }
  }

  function hide() {
    root.visible = false
    formsStack.visible = false
  }

  StackView {
    id: formsStack

    function popOneOrClose() {
      formsStack.pop()

      if ( formsStack.depth <= 1 )
        root.closed() // this is the top most form, we want to keep it instantiated, just invisible
    }

    anchors.fill: parent
  }

  Component {
    id: formComponent

    Forms.FormWrapper {
      id: wrapper

      project: root.project

      previewHeight: root.previewHeight

      onClosed: formsStack.popOneOrClose()
      onEditGeometry: root.geometryEditingStarted( StackView.index )

      onCreateFeature: {
        // TODO
      }
    }
  }
}
