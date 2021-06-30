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
   * FormsStackManager component is responsible for viewing feature forms, both preview and fullscreen form
   * It contains a stackview where instances of FormWrapper are pushed. Latest form is not destroyed, but reused
   * for next feature to reduce a cost of initializing form and AttributeController each time user selects feature.
   */

  property var project
  property real previewHeight
  property DigitizingController digitizingController

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

  function updateFeatureGeometry() {
    let form = _getActiveForm()

    if ( form )
      form.updateFeatureGeometry( activeFormIndex )
  }

  function geometryEditingFinished( formState ) {
    let form = _getActiveForm()

    if ( form ) {
      form.formState = formState
      form.panelState = "form"
    }
  }

  function closeDrawer() {
    let form = _getActiveForm()

    if ( form )
      form.closeDrawer()
  }

  function geometryEditingStarted( formIndex ) {
    activeFormIndex = formIndex
    hide()
    editGeometry()
  }

  function reload() {
    formsStack.clear() // removes all objects due to Qt parent system
  }

  function hide() {
    root.visible = false
    formsStack.visible = false
  }

  function openLinkedFeature( linkedFeature ) {
    let props = {
      featureLayerPair: linkedFeature,
      formState: "ReadOnly",
      panelState: "form"
    }

    let latest = formsStack.push( formComponent, props )
    root.activeFormIndex = latest.StackView.index
  }

  function createLinkedFeature( parentFeature, relation ) {
    let newFeaturePair = digitizingController.featureWithoutGeometry( relation.referencingLayer )

    let props = {
      featureLayerPair: newFeaturePair,
      formState: "Add",
      panelState: "form",
      parentFeatureLayerPair: parentFeature,
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
      onOpenLinkedFeature: root.openLinkedFeature( linkedFeature )
      onCreateLinkedFeature: root.createLinkedFeature( parentFeature, relation )
    }
  }
}
