/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import lc 1.0

import "../components"
import Input 0.1 as Input

Page {
  id: root

  /**
   * When feature in the form is saved.
   */
  signal saved

  /**
   * When the form is about to be closed by closeButton or deleting a feature.
   */
  signal canceled

  /**
   * Signal emited when relation editor requests to open child feature form
   */
  signal openLinkedFeature( var linkedFeature )

  /**
   * Signal emited when relation editor requests to create child feature and open its form
   */
  signal createLinkedFeature( var parentController, var relation )

  signal splitGeometryRequested()
  signal redrawGeometryRequested( var layerPair )
  signal editGeometryRequested( var layerPair )

  /**
   * Active project.
   */
  property Input.Project project

  /**
   * Controller
   */
  property AttributeController controller

  implicitWidth: ApplicationWindow.window?.width ?? 0
  implicitHeight: ApplicationWindow.window?.height ?? 0

  states: [
    State {
      name: "readOnly"
    },
    State {
      name: "edit"
    },
    State {
      name: "add"
    }
  ]

  background: Rectangle {
    color: __style.lightGreenColor
  }

  header: MMHeader {
    title: {
      if ( root.state === "add" ) return qsTr( "New feature" )
      else if ( root.state === "edit" ) return qsTr( "Edit feature" )
      return __inputUtils.featureTitle( root.controller.featureLayerPair, __activeProject.qgsProject )
    }

    rightMarginShift: saveButton.visible ? saveButton.width : 0

    onBackClicked: root.rollbackAndClose()

    MMRoundButton {
      id: saveButton

      anchors.right: parent.right
      anchors.verticalCenter: parent.verticalCenter
      anchors.rightMargin: __style.pageMargins

      visible: root.state === "add" || root.state === "edit"

      iconSource: __style.checkmarkIcon
      iconColor: __style.forestColor

      bgndColor: __style.grassColor

      onClicked: root.save()
    }
  }

  ColumnLayout {
    anchors.fill: parent

    MMFormTabBar {
      id: tabBar

      Layout.alignment: Qt.AlignHCenter
      Layout.maximumWidth: __style.maxPageWidth

      visible: root.controller.hasTabs

      tabButtonsModel: root.controller.attributeTabProxyModel

      onCurrentIndexChanged: formSwipe.setCurrentIndex( tabBar.currentIndex )
    }

    SwipeView {
      id: formSwipe

      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.alignment: Qt.AlignHCenter
      Layout.maximumWidth: __style.maxPageWidth

      clip: true

      onCurrentIndexChanged: tabBar.setCurrentIndex( formSwipe.currentIndex )

      Repeater {
        id: swipeViewRepeater

        model: root.controller.attributeTabProxyModel

        Item {
          id: pageDelegate

          property int tabIndex: model.TabIndex // from the repeater

          ListView {

            anchors {
              fill: parent
              leftMargin: __style.pageMargins
              rightMargin: __style.pageMargins
            }

            model: swipeViewRepeater.model.attributeFormProxyModel( pageDelegate.tabIndex )

            clip: true
            spacing: internal.formSpacing

            header: Rectangle {
              opacity: 1 // invisible
              height: 20 * __dp
            }

            section {
              property: "Group"
              delegate: sectionDelegate
              labelPositioning: ViewSection.CurrentLabelAtStart | ViewSection.InlineLabels
            }

            delegate: editorDelegate

            footer: Rectangle {
              opacity: 1 // invisible
              height: 20 * __dp
            }
          }

        }
      }
    }
  }

  footer: MMToolbar {

//    TODO: visible: !root.layerIsReadOnly

    maxButtonsInToolbar: 3

    ObjectModel {
      id: readStateButtons

      MMToolbarLongButton {
        text: qsTr( "Edit feature" );

        iconSource: __style.editIcon;

        onClicked: {
          root.state = "edit"
        }
      }
    }

    ObjectModel {
      id: addStateButtons // edit buttons are the same

      MMToolbarButton {
        text: qsTr( "Delete" )
        iconSource: __style.deleteIcon
        onClicked: deleteDialog.open()
      }

      MMToolbarButton {
        text: qsTr( "Edit geometry" )
        iconSource: __style.editIcon
        onClicked: root.editGeometryRequested( root.controller.featureLayerPair )
      }

      MMToolbarButton {
        text: qsTr( "Redraw geometry" )
        iconSource: __style.editIcon
        onClicked: root.redrawGeometryRequested( root.controller.featureLayerPair )
      }

      MMToolbarButton {
        text: qsTr( "Split geometry" )
        iconSource: __style.editIcon
        onClicked: root.splitGeometryRequested()
      }
    }


    model: root.state === "readOnly" ? readStateButtons : addStateButtons
  }

  Component {
    id: sectionDelegate

    Item {

      property string sectionTitle: section

      height: section ? 76 * __dp : 0
      width: ListView.view.width

      // section bgnd
      Rectangle {
        anchors.fill: parent;
        color: __style.lightGreenColor;
      }

      Text {
        id: sectionTitle

        text: section
        font: __style.h3
        color: __style.forestColor

        topPadding: internal.formSpacing
        bottomPadding: internal.formSpacing
      }
    }
  }

  Component {
    id: editorDelegate

    Item {

      width: ListView.view.width
      implicitHeight: childrenRect.height

      // In future, better to filter such fields in the field proxy model instead
      visible: Type !== FormItem.Invalid && Type !== FormItem.Container

      Loader {
        id: formEditorsLoader

        //
        // Maybe one day we could use DelegateChooser instead of this hack-ish approach, see:
        // https://doc.qt.io/qt-6/qml-qt-labs-qmlmodels-delegatechooser.html
        //

        width: parent.width

        property var fieldValue: model.RawValue
        property bool fieldValueIsNull: model.RawValueIsNull

        property var field: model.Field
        property var fieldIndex: model.FieldIndex
        property var fieldWidget: model.EditorWidget
        property var fieldConfig: model.EditorWidgetConfig

        property bool fieldIsReadOnly: root.state === "readOnly" || !AttributeEditable
        property bool fieldShouldShowTitle: model.ShowName

        property string fieldTitle: model.Name
        property string fieldErrorMessage: model.ValidationStatus === FieldValidator.Error ? model.ValidationMessage : ""
        property string fieldWarningMessage: model.ValidationStatus === FieldValidator.Warning ? model.ValidationMessage : ""

        property var fieldActiveProject: root.project
        property var fieldAssociatedRelation: model.Relation
        property var fieldFeatureLayerPair: root.controller.featureLayerPair
        property string fieldHomePath: root.project ? root.project.homePath : "" // for photo editor

        property bool fieldRememberValueSupported: root.controller.rememberAttributesController.rememberValuesAllowed && root.state === "add" && model.EditorWidget !== "Hidden" && Type === FormItem.Field
        property bool fieldRememberValueState: model.RememberValue ? true : false

        active: fieldWidget !== 'Hidden'

        Keys.forwardTo: backHandler

        source: {
          if ( model.EditorWidget !== undefined ) {
            return __inputUtils.getFormEditorType( model.EditorWidget, model.EditorWidgetConfig, model.Field )
          }

          return ''
        }
      }

      Connections {
        target: formEditorsLoader.item
        ignoreUnknownSignals: true

        function onEditorValueChanged( newVal, isNull ) {
          model.AttributeValue = isNull ? undefined : newVal
        }

        function onRememberValueBoxClicked( state ) {
          model.RememberValue = state
        }

        // TODO: support for relations
      }

      Connections {
        target: root.controller

        // Important for relation form editors
        function onFeatureLayerPairChanged() {
          if ( formEditorsLoader.item && formEditorsLoader.item.featureLayerPairChanged )
          {
            formEditorsLoader.item.featureLayerPairChanged()
          }
        }

        // Important for value relation form editors
        function onFormRecalculated() {
          if ( formEditorsLoader.item && formEditorsLoader.item.reload )
          {
            formEditorsLoader.item.reload()
          }
        }
      }

      Connections {
        target: root
        ignoreUnknownSignals: true

        function onSaved() {
          if ( formEditorsLoader.item && typeof formEditorsLoader.item.callbackOnSave === "function" ) {
            formEditorsLoader.item.callbackOnFormSaved()
          }
        }

        function onCanceled() {
          if ( formEditorsLoader.item && typeof formEditorsLoader.item.callbackOnCancel === "function" ) {
            formEditorsLoader.item.callbackOnFormCanceled()
          }
        }
      }
    }
  }

  MessageDialog {
    id: saveChangesDialog

    visible: false
    title: qsTr( "Unsaved changes" )
    text: qsTr( "Do you want to save changes?" )
    buttons: MessageDialog.Yes | MessageDialog.No | MessageDialog.Cancel

    onButtonClicked: function( clickedButton ) {
      if ( clickedButton === MessageDialog.Yes ) {
        root.save()
      }
      else if ( clickedButton === MessageDialog.No ) {
        root.rollbackAndClose()
      }
      saveChangesDialog.close()
    }
  }

  MessageDialog {
    id: deleteDialog

    title: qsTr( "Delete feature" )
    text: qsTr( "Are you sure you want to delete this feature?" )
    buttons: MessageDialog.Yes | MessageDialog.No

    onButtonClicked: function( clickedButton ) {
      if ( clickedButton === MessageDialog.Yes ) {
        root.controller.deleteFeature()
        root.canceled()
        deleteDialog.close()
      }

      deleteDialog.close()
    }
  }

  MessageDialog {
    id: editingFailedDialog

    title: qsTr( "Saving failed" )
    text: qsTr( "Failed to save changes. This should not happen normally. Please restart the app and try again — if that does not help, please contact support." )
    buttons: MessageDialog.Close

    onButtonClicked: close()
  }

  Item {
    id: backHandler

    focus: true
    Keys.onReleased: function( event ) {
      if ( event.key === Qt.Key_Back || event.key === Qt.Key_Escape ) {
        if ( root.controller.hasAnyChanges )  {
          saveChangesDialog.open()
        }
        else {
          root.rollbackAndClose()
        }
        event.accepted = true;
      }
    }

    onVisibleChanged: function( visible ) {
      if ( visible )
        backHandler.forceActiveFocus()
    }
  }

  Connections {
    target: Qt.inputMethod

    function onVisibleChanged() {
      Qt.inputMethod.commit()
    }
  }

  Connections {
    target: root.controller

    function onChangesCommited() {
      root.saved()
    }

    function onCommitFailed() {
      editingFailedDialog.open()
    }
  }

  function reset() {
    master.reset()
  }

  function save() {
    if ( controller.hasValidationErrors )
    {
      console.log( qsTr( 'Can not save the form, there are validation errors' ) )
      __inputUtils.showNotification( qsTr( 'Feature could not be saved, please check all required fields' ) )

      // In future we could navigate user to a field that contains validation error
      return
    }

    parent.focus = true
    controller.save()
  }

  function rollbackAndClose() {
    // remove feature if we are in "add" mode and it already has valid ID
    // it was saved to prefill relation reference field in child layer
    let featureId = root.controller.featureLayerPair.feature.id
    let shouldRemoveFeature = root.state === "add" && __inputUtils.isFeatureIdValid( featureId )

    if ( shouldRemoveFeature ) {
      root.controller.deleteFeature()
    }

    parent.focus = true

    // rollback all changes if the layer is still editable
    root.controller.rollback()

    root.canceled()
  }

  /**
   * This is a relay to forward private signals to internal components.
   */
  QtObject {
    id: master

    /**
     * This signal is emitted whenever the state of Flickables and TabBars should
     * be restored.
     */
    signal reset
  }

  QtObject {
    id: internal

    property real formSpacing: 20 * __dp
  }
}
