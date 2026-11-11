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

import mm 1.0 as MM
import qgs 1.0 as QGS
import MMInput

import "../components" as MMComponents
import "./components" as MMFormComponents
import "../dialogs"


Page {
  id: root

  // Qt 6.9+ Page automatically adds safe area margins as padding.
  // MMPageHeader and MMToolbar already handle safe areas manually, so
  // we set them to zero to avoid double-applying the insets.
  topPadding: 0
  bottomPadding: 0
  leftPadding: 0
  rightPadding: 0

  /**
   * When feature in the form is saved.
   */
  signal saved

  /**
   * When the form is about to be closed by closeButton or deleting a feature.
   */
  signal canceled

  /**
   * Signal emitted when relation editor requests to open child feature form
   */
  signal openLinkedFeature( var linkedFeature )

  /**
   * Signal emitted when relation editor requests to create child feature and open its form
   */
  signal createLinkedFeature( var parentController, var relation )

  signal editGeometryRequested( var layerPair )

  /**
   * Active project.
   */
  property QGS.Project project

  /**
   * Controller
   */
  property MM.AttributeController controller

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
      name: "multiEdit"
    },
    State {
      name: "add"
    }
  ]

  Keys.onReleased: function( event ) {
    if ( event.key === Qt.Key_Back || event.key === Qt.Key_Escape ) {
      root.requestClose()
      event.accepted = true;
    }
  }

  property bool layerIsReadOnly: true
  property bool layerIsSpatial: true

  background: Rectangle {
    color: __style.lightGreenColor
  }

  header: MMComponents.MMPageHeader {

    onBackClicked: root.requestClose()


    title: {
      // the recording indicator takes the room of the title
      if ( audioRecorder.isRecording ) return ""
      if ( root.state === "add" ) return qsTr( "New feature" )
      else if ( root.state === "edit" ) return qsTr( "Edit feature" )
      else if ( root.state === "multiEdit" ) return qsTr( "Edit selected features" )
      return __inputUtils.featureTitle( root.controller.featureLayerPair, __activeProject.qgsProject )
    }

    rightItemContent: Row {

      anchors.verticalCenter: parent.verticalCenter

      spacing: __style.margin8

      MMFormComponents.MMFormRecordingIndicator {
        id: recordingIndicator

        anchors.verticalCenter: parent.verticalCenter

        visible: audioRecorder.isRecording
        duration: audioRecorder.duration

        onClicked: internal.showRecordingField()
        onStopClicked: audioRecorder.stopRecording()
      }

      MMComponents.MMRoundButton {

        anchors.verticalCenter: parent.verticalCenter

        visible: root.state === "add" || root.state === "edit" || root.state === "multiEdit"

        iconSource: __style.checkmarkIcon
        iconColor: controller.hasValidationErrors ? __style.grapeColor : __style.forestColor
        bgndColor: controller.hasValidationErrors ? __style.negativeColor : __style.grassColor

        onClicked: root.save()
      }
    }
  }

  ColumnLayout {
    anchors.fill: parent

    MMFormComponents.MMFormTabBar {
      id: tabBar

      Layout.topMargin: __style.margin10
      Layout.bottomMargin: __style.margin10
      Layout.alignment: Qt.AlignHCenter
      Layout.fillWidth: true
      Layout.maximumWidth: Math.min(__style.maxPageWidth, root.width)

      model: root.controller.attributeTabProxyModel

      visible: root.controller.hasTabs

      onCurrentIndexChanged: formSwipe.setCurrentIndex( tabBar.currentIndex )
    }

    SwipeView {
      id: formSwipe

      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.alignment: Qt.AlignHCenter
      Layout.maximumWidth: __style.maxPageWidth

      clip: true

      onCurrentIndexChanged: tabBar.currentIndex = formSwipe.currentIndex

      Repeater {
        id: swipeViewRepeater

        model: root.controller.attributeTabProxyModel

        Item {
          id: pageDelegate

          property int tabIndex: model.TabIndex // from the repeater
          property alias listView: formListView

          MMComponents.MMListView {
            id: formListView

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
              labelPositioning: ViewSection.InlineLabels
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

  footer: MMComponents.MMToolbar {

    visible: !root.layerIsReadOnly && __activeProject.projectRole !== "reader" && root.state !== "multiEdit"

    ObjectModel {
      id: readStateButtons

      MMComponents.MMToolbarButton {
        text: qsTr( "Edit feature" );

        iconSource: __style.editCircleIcon
        iconColor: __style.forestColor

        onClicked: root.state = "edit"
      }
    }

    ObjectModel {
      id: addStateButtons // edit buttons are the same

      MMComponents.MMToolbarButton {
        text: qsTr( "Delete" )
        iconSource: __style.deleteIcon
        iconColor: root.layerIsSpatial ? __style.polarColor : __style.grapeColor
        bgColor: root.layerIsSpatial ? __style.grassColor : __style.negativeColor
        onClicked: deleteDialog.open()
      }

      MMComponents.MMToolbarButton {
        id: editGeometry
        text: qsTr( "Edit geometry" )
        iconSource: __style.editIcon
        visible: root.layerIsSpatial && __activeProject.projectRole !== "reader"
        onClicked: internal.editGeometry()
      }
    }


    model: root.state === "readOnly" ? readStateButtons : addStateButtons
  }

  Component {
    id: sectionDelegate

    Rectangle {

      height: section ? childrenRect.height : 0
      width: ListView.view.width

      color: __style.lightGreenColor

      MMComponents.MMText {
        text: section
        font: __style.h3
        color: __style.forestColor

        width: parent.width
        wrapMode: Text.Wrap
        elide: Text.ElideRight

        topPadding: internal.formSpacing
        bottomPadding: internal.formSpacing

        maximumLineCount: 3
      }
    }
  }

  Component {
    id: editorDelegate

    Item {

      width: ListView.view.width
      implicitHeight: childrenRect.height

      // In future, better to filter such fields in the field proxy model instead
      visible: Type !== MM.FormItem.Invalid && Type !== MM.FormItem.Container

      Loader {
        id: formEditorsLoader

        //
        // Maybe one day we could use DelegateChooser instead of this hack-ish approach, see:
        // https://doc.qt.io/qt-6/qml-qt-labs-qmlmodels-delegatechooser.html
        //

        width: parent.width

        property var fieldValue: model.RawValue
        property bool fieldValueIsNull: model.RawValueIsNull ?? true
        property bool fieldHasMixedValues: model.HasMixedValues ?? false
        property bool fieldFormIsMultiEdit: root.state === "multiEdit"

        property var field: model.Field
        property var fieldIndex: model.FieldIndex
        property string fieldItemId: model.FormItemId
        property var fieldWidget: model.EditorWidget
        property var fieldConfig: model.EditorWidgetConfig

        property bool fieldFormIsReadOnly: root.state === "readOnly"
        property bool fieldIsEditable: AttributeEditable

        property bool fieldShouldShowTitle: model.ShowName

        property string fieldTitle: model.Name
        property string fieldErrorMessage: model.ValidationStatus === MM.FieldValidator.Error ? model.ValidationMessage : ""
        property string fieldWarningMessage: model.ValidationStatus === MM.FieldValidator.Warning ? model.ValidationMessage : ""

        property var fieldActiveProject: root.project
        property var fieldAssociatedRelation: model.Relation
        property MM.AttributeController fieldController: root.controller
        property string fieldHomePath: root.project ? root.project.homePath : "" // for photo editor
        property AudioRecorderController fieldAudioRecorder: audioRecorder // for audio editor, outlives the delegate

        property bool fieldRememberValueSupported: root.controller.rememberAttributesController.rememberValuesAllowed && root.state === "add" && model.EditorWidget !== "Hidden" && Type === MM.FormItem.Field
        property bool fieldRememberValueState: model.RememberValue ? true : false

        active: fieldWidget !== 'Hidden'

        source: {
          if ( model.EditorWidget !== undefined ) {
            return __inputUtils.getFormEditorType( model.EditorWidget, model.EditorWidgetConfig, model.Field, model.Relation, model.Name, fieldFormIsMultiEdit )
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

        function onCreateLinkedFeature( parentFeature, relation ) {
          internal.stopRecordingBeforeLeaving()

          let parentHasValidId = __inputUtils.isFeatureIdValid( parentFeature.feature.id )

          if ( parentHasValidId ) {
            // parent feature in this case already have valid id, so we can open new form
            root.createLinkedFeature( root.controller, relation )
          }
          else {
            // parent feature does not have a valid ID yet, we need to save it and acquire ID
            root.controller.acquireId()
            root.createLinkedFeature( root.controller, relation )
          }
        }

        function onOpenLinkedFeature( linkedFeature ) {
          internal.stopRecordingBeforeLeaving()
          root.openLinkedFeature( linkedFeature )
        }
      }

      Connections {
        target: root.controller

        // Important for value relation form editors
        function onFormRecalculated() {
          if ( formEditorsLoader.item && formEditorsLoader.item.hotReload )
          {
            formEditorsLoader.item.hotReload()
          }
        }
      }

      Connections {
        target: root
        ignoreUnknownSignals: true

        function onSaved() {
          if ( formEditorsLoader.item && typeof formEditorsLoader.item.callbackOnFormSaved === "function" ) {
            formEditorsLoader.item.callbackOnFormSaved()
          }
        }

        function onCanceled() {
          if ( formEditorsLoader.item && typeof formEditorsLoader.item.callbackOnFormCanceled === "function" ) {
            formEditorsLoader.item.callbackOnFormCanceled()
          }
        }
      }
    }
  }

  MMFormSaveChangesDialog {
    id: saveChangesDialog

    onSaveChanges: root.save()
    onDiscardChanges: root.rollbackAndClose()
  }

  MMFormDeleteFeatureDialog {
    id: deleteDialog

    onDeleteFeature: {
      audioRecorder.discardRecording()
      root.controller.deleteFeature()
      root.canceled()
    }
  }

  MMFormEditFailedDialog {
    id: editingFailedDialog
  }

  MMMicrophonePermissionDialog {
    id: microphonePermissionDialog
  }

  // lives here and not in the audio editor, so recording continues while the editor is scrolled out of view
  AudioRecorderController {
    id: audioRecorder

    attributeController: root.controller

    // A pending save continues from the result of the recording and not from isRecordingChanged,
    // which comes first: a failed recording must not save the feature
    onRecordingFinished: function( fieldItemId ) {
      internal.continueAfterRecording()
    }

    // the feature is not saved without its recording, the form stays open
    onRecordingFailed: function( fieldItemId, message ) {
      internal.saveWhenRecordingStops = false
      internal.editGeometryWhenRecordingStops = false
      __notificationModel.addError( message )
    }

    onRecordingInterrupted: function( fieldItemId, message ) {
      __notificationModel.addWarning( message )
    }

    // only one field records at a time
    onRecordingBusy: function( fieldItemId ) {
      if ( audioRecorder.activeFieldTitle ) {
        __notificationModel.addWarning( qsTr( "Stop recording “%1” first" ).arg( audioRecorder.activeFieldTitle ) )
      }
      else {
        __notificationModel.addWarning( qsTr( "Stop the current recording first" ) )
      }
      recordingIndicator.nudge()
    }

    onMaximumDurationWarning: {
      __notificationModel.addWarning( qsTr( "Recording stops automatically at %1 minutes" ).arg( Math.round( audioRecorder.maximumDuration / 60000 ) ) )
    }

    onMicrophonePermissionDenied: microphonePermissionDialog.open()
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
    // the recording is assigned to its field once it is finalised, then the save continues
    if ( audioRecorder.isRecording ) {
      internal.saveWhenRecordingStops = true
      audioRecorder.stopRecording()
      return
    }

    if ( controller.hasValidationErrors )
    {
      __notificationModel.addError( qsTr( 'Feature could not be saved, please check all required fields' ) )

      // In future we could navigate user to a field that contains validation error
      return
    }

    parent.focus = true
    controller.save()
  }

  function requestClose() {
    // A running recording counts as an unsaved change: "save" stops and keeps it with the feature,
    // "no" discards it together with the other changes
    if ( audioRecorder.isRecording || root.controller.hasAnyChanges ) {
      saveChangesDialog.open()
    }
    else {
      root.rollbackAndClose()
    }
  }

  function rollbackAndClose() {
    // a running recording is discarded with the other changes, a save waiting for it is dropped
    internal.saveWhenRecordingStops = false
    internal.editGeometryWhenRecordingStops = false
    audioRecorder.discardRecording()

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

    property bool saveWhenRecordingStops: false
    property bool editGeometryWhenRecordingStops: false

    // continues the save or the geometry editing that waited for the recording to be finalised
    function continueAfterRecording() {
      if ( saveWhenRecordingStops ) {
        saveWhenRecordingStops = false
        editGeometryWhenRecordingStops = false
        root.save()
      }
      else if ( editGeometryWhenRecordingStops ) {
        editGeometryWhenRecordingStops = false
        root.editGeometryRequested( root.controller.featureLayerPair )
      }
    }

    // The form gets the edited feature back with the values it has when the editing starts. A running recording
    // is stopped and kept first, so its file is among them and is still in the field when the user comes back
    function editGeometry() {
      if ( audioRecorder.isRecording ) {
        editGeometryWhenRecordingStops = true
        stopRecordingBeforeLeaving()
        return
      }

      root.editGeometryRequested( root.controller.featureLayerPair )
    }

    // switches to the tab with the recording field and scrolls to it
    function showRecordingField() {
      const fieldItemId = audioRecorder.activeFieldItemId
      if ( !fieldItemId ) {
        return
      }

      for ( let tab = 0; tab < swipeViewRepeater.count; tab++ ) {
        const page = swipeViewRepeater.itemAt( tab )
        const formModel = page.listView.model

        for ( let row = 0; row < formModel.rowCount(); row++ ) {
          if ( formModel.data( formModel.index( row, 0 ), MM.AttributeFormModel.FormItemId ) === fieldItemId ) {
            tabBar.currentIndex = tab
            page.listView.positionViewAtIndex( row, ListView.Center )
            return
          }
        }
      }

      // the field is not in the form at the moment, e.g. hidden by its visibility expression
      __notificationModel.addWarning( qsTr( "The field being recorded is hidden at the moment" ) )
    }

    // The header indicator is the only sign of a running recording and only one form records at a time,
    // so the recording does not continue when the form is hidden or covered by another form
    function stopRecordingBeforeLeaving() {
      audioRecorder.interruptRecording( qsTr( "Recording stopped because you left the form" ) )
    }
  }
}
