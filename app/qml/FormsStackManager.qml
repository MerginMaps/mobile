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

import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "./form" as Forms
import "."

Item {
  id: root

  /*
   * FormsStackManager component is responsible for viewing feature forms, both preview and fullscreen form
   * It contains a stackview where instances of FormWrapper are pushed. Latest form is not destroyed, but reused
   * for next feature to reduce a cost of initializing form and AttributeController each time user selects feature.
   */

  property var project
  property real previewHeight

  property int activeFormIndex: 0

  signal closed()
  signal createLinkedFeatureRequested( var parentController, var relation )

  // TODO: (next PR) remove unused functions

  function openForm( pair, formState, panelState ) {
    if ( formsStack.depth === 0 )
    {
      let props = {
        featureLayerPair: pair,
        formState: formState,
        panelState: panelState
      }

      var latest = formsStack.push( formComponent, props )
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

  function _getActiveForm() {
    if ( root.activeFormIndex >= 0 && root.activeFormIndex < formsStack.depth ) {
      return formsStack.get( activeFormIndex )
    }
    else console.error( "FormsStackManager: Opted for invalid form index" )
    return null
  }

  function setFeaturePair( pair ) {
    let form = _getActiveForm()

    if ( form )
      form.featureLayerPair = pair
  }

  function getFeaturePair() {
    let form = _getActiveForm()

    if ( form )
      return form.featureLayerPair
  }

  function isNewFeature() {
    let form = _getActiveForm()

    if ( form )
      return form.isNewFeature()
  }

  function closeDrawer() {
    let form = _getActiveForm()

    if ( form )
      form.closeDrawer()
  }

  function reload() {
    formsStack.clear() // removes all objects thanks to Qt parent system
  }

  function hide() {
    root.visible = false
    formsStack.visible = false
  }

  function openLinkedFeature( linkedFeature ) {
    let props = {
      featureLayerPair: linkedFeature,
      formState: "readOnly",
      panelState: "form"
    }

    let latest = formsStack.push( formComponent, props )
    root.activeFormIndex = latest.StackView.index
  }

  function addLinkedFeature( newPair, parentController, relation ) {
    let props = {
      featureLayerPair: newPair,
      formState: "add",
      panelState: "form",
      parentController: parentController,
      linkedRelation: relation
    }

    let latest = formsStack.push( formComponent, props )
    root.activeFormIndex = latest.StackView.index
  }

  StackView {
    id: formsStack

    function popOneOrClose() {
      formsStack.pop()

      if ( formsStack.depth <= 1 )
        root.closed() // this is the top most form, we want to keep it instantiated, just invisible
    }

    focus: true

    anchors.fill: parent
  }

  Component {
    id: formComponent

    Forms.FormWrapper {
      id: wrapper

      project: root.project

      previewHeight: root.previewHeight

      onClosed: formsStack.popOneOrClose()
      onOpenLinkedFeature: root.openLinkedFeature( linkedFeature )
      onCreateLinkedFeature: root.createLinkedFeatureRequested( parentController, relation )
    }
  }
}
