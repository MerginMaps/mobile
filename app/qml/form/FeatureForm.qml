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
import QtQml.Models
import QtQml

import lc 1.0
import "../components"
import Input 0.1 as Input

Item {
  /**
   * When feature in the form is saved.
   */
  signal saved

  /**
   * When the form is about to be closed by closeButton or deleting a feature.
   */
  signal canceled

  /**
   * When edit operation failed.
   */
  signal editingFailed

  /**
   * Signal emited when relation editor requests to open child feature form
   */
  signal openLinkedFeature( var linkedFeature )

  /**
   * Signal emited when relation editor requests to create child feature and open its form
   */
  signal createLinkedFeature( var parentController, var relation )

   /**
    * A handler for extra events in externalSourceWidget.
    */
  property var externalResourceHandler: QtObject {

        /**
         * Called when clicked on the camera icon to capture an image.
         * \param itemWidget editorWidget for modified field to send valueChanged signal.
         */
        property var capturePhoto: function captureImage(itemWidget) {
        }

        /**
         * Called when clicked on the gallery icon to choose a file in a gallery.
         * \param itemWidget editorWidget for modified field to send valueChanged signal.
         */
        property var chooseImage: function chooseImage(itemWidget) {
        }

        /**
          * Called when clicked on the photo image. Suppose to be used to bring a bigger preview.
          * \param imagePath Absolute path to the image.
          */
        property var previewImage: function previewImage(imagePath) {
        }

        /**
          * Called when clicked on the trash icon. Suppose to delete the value and optionally also the image.
          * \param itemWidget editorWidget for modified field to send valueChanged signal.
          * \param imagePath Absolute path to the image.
          */
        property var removeImage: function removeImage(itemWidget, imagePath) {
        }

        /**
          * Called when clicked on the OK icon after taking a photo with the Photo panel.
          * \param itemWidget editorWidget for modified field to send valueChanged signal.
          * \param prefixToRelativePath Together with the value creates absolute path
          * \param value Relative path of taken photo.
          */
        property var confirmImage: function confirmImage(itemWidget, prefixToRelativePath, value) {
          itemWidget.image.source = prefixToRelativePath + "/" + value
          itemWidget.editorValueChanged(value, value === "" || value === null)
        }
    }

  /**
   * Active project.
   */
  property Input.Project project

  /**
   * Controller
   */
  property AttributeController controller

  /**
   * View for extra components like value relation page, relations page, etc.
   */
  property StackView extraView

  /**
   * Predefined form styling
   */
  property FeatureFormStyling style: FeatureFormStyling {}

  id: form

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

  function cancel() {
    // remove feature if we are in "add" mode and it already has valid ID
    // it was saved to prefill relation reference field in child layer
    let featureId = form.controller.featureLayerPair.feature.id
    let shouldRemoveFeature = form.state === "add" && __inputUtils.isFeatureIdValid( featureId )

    if ( shouldRemoveFeature ) {
      form.controller.deleteFeature()
    }

    // rollback all changes if the layer is still editable
    form.controller.rollback()

    canceled()
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

  StackView {
    id: formView

    anchors.fill: parent

    initialItem: container
  }

  Rectangle {
    id: container

    clip: true
    color: form.style.tabs.backgroundColor

    width: formView.width
    height: formView.height

    Flickable {
      id: flickable
      anchors {
        left: container.left
        right: container.right
        leftMargin: form.style.fields.outerMargin
        rightMargin: form.style.fields.outerMargin
      }
      height: form.controller.hasTabs ? tabRow.height : 0

      flickableDirection: Flickable.HorizontalFlick
      contentWidth: tabRow.width

      // Tabs
      TabBar {
        id: tabRow
        visible: form.controller.hasTabs
        height: form.style.tabs.height
        spacing: form.style.tabs.spacing

        background: Rectangle {
          anchors.fill: parent
          color: form.style.tabs.backgroundColor
        }

        Connections {
          target: master
          function onReset() {
            tabRow.currentIndex = 0
          }
        }

        Connections {
          target: swipeView
          function onCurrentIndexChanged() {
            tabRow.currentIndex = swipeView.currentIndex
          }
        }

        Repeater {
          model: form.controller.attributeTabProxyModel

          TabButton {
            id: tabButton
            text: Name
            leftPadding: 8 * __dp
            rightPadding: 8 * __dp
            anchors.bottom: parent.bottom
            focusPolicy: Qt.NoFocus

            width: leftPadding + rightPadding
            height: form.style.tabs.buttonHeight

            contentItem: Text {
              // Make sure the width is derived from the text so we can get wider
              // than the parent item and the Flickable is useful
              Component.onCompleted: {
                tabButton.width = tabButton.width + paintedWidth
                if (tabRow.currentIndex == index)
                  tabButton.checked = true
              }

              width: paintedWidth
              text: tabButton.text
              color: !tabButton.enabled ? form.style.tabs.disabledColor : tabButton.down ||
                                          tabButton.checked ? form.style.tabs.activeColor : form.style.tabs.normalColor
              font.weight: Font.DemiBold
              font.underline: tabButton.checked ? true : false
              font.pixelSize: form.style.tabs.tabLabelPixelSize
              opacity: tabButton.checked ? 1 : 0.5

              horizontalAlignment: Text.AlignHCenter
              verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
              color: !tabButton.enabled ? form.style.tabs.disabledBackgroundColor : tabButton.down ||
                                                 tabButton.checked ? form.style.tabs.activeBackgroundColor : form.style.tabs.normalBackgroundColor
            }
          }
        }
      }
    }

    SwipeView {
      id: swipeView
      currentIndex: form.controller.hasTabs ? tabRow.currentIndex : 0

      //
      // Known limitation, we can not make swipeview interactive because of https://bugreports.qt.io/browse/QTBUG-109124
      // It clashes with slider editors, see https://github.com/MerginMaps/input/issues/2411
      //
      interactive: false

      anchors {
        top: flickable.bottom
        left: container.left
        right: container.right
        bottom: container.bottom
     }

      Repeater {
        //One page per tab in tabbed forms, 1 page in auto forms

        model: form.controller.attributeTabProxyModel
        id: swipeViewRepeater

        Item {
          id: formPage
          property int tabIndex: model.TabIndex

          // The main form content area
          Rectangle {
            anchors.fill: formPage
            color: form.style.backgroundColor
            opacity: form.style.backgroundOpacity
          }

          ListView {
            id: content
            anchors.fill: formPage
            clip: true
            spacing: form.style.group.spacing
            section.property: "Group"
            section.labelPositioning: ViewSection.CurrentLabelAtStart | ViewSection.InlineLabels
            section.delegate: Component {

              // section header: group box name
              Item {
                id: headerContainer
                width: ListView.view.width
                height: section === "" ? 0 : form.style.group.height + form.style.group.spacing // add space after section header

                Rectangle {
                  width: headerContainer.width
                  height: section === "" ? 0 : form.style.group.height
                  color: form.style.group.marginColor
                  anchors.top: headerContainer.top

                  Rectangle {
                    anchors.fill: parent
                    anchors {
                      leftMargin: form.style.group.leftMargin
                      rightMargin: form.style.group.rightMargin
                      topMargin: form.style.group.topMargin
                      bottomMargin: form.style.group.bottomMargin
                    }
                    color: form.style.group.backgroundColor

                    Text {
                      anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }
                      font.bold: true
                      font.pixelSize: form.style.group.fontPixelSize
                      text: section
                      color: form.style.group.fontColor
                    }
                  }
                }
              }
            }


            Connections {
              target: master
              function onReset() {
                content.contentY = 0
              }
            }


            model: swipeViewRepeater.model.attributeFormProxyModel(formPage.tabIndex)

            delegate: fieldItem

            header: Rectangle {
              opacity: 1
              height: form.style.group.spacing
            }

            footer: Rectangle {
              opacity: 1
              height: 2 * form.style.group.spacing
            }
          }
        }
      }
    }

    // Borders
    Rectangle {
      width: container.width
      height: form.style.tabs.borderWidth
      anchors.top: flickable.top
      color: form.style.tabs.borderColor
      visible: flickable.height
    }

    Rectangle {
      width: container.width
      height: form.style.tabs.borderWidth
      anchors.bottom: flickable.bottom
      color: form.style.tabs.borderColor
      visible: flickable.height
    }
  }

  /**
   * A field editor
   */
  Component {
    id: fieldItem

    Item {
      id: fieldContainer

      // TODO: filter such fields in field proxy model instead
      property bool shouldBeVisible: Type === FormItem.Field || Type === FormItem.Relation

      visible: shouldBeVisible

      // We also need to set height to zero if Type is not field otherwise children created blank space in form
      height: shouldBeVisible ? childrenRect.height : 0
      width: ListView.view.width

      Item {
        id: paddedEditorField

        anchors {
          left: fieldContainer.left
          right: fieldContainer.right
          leftMargin: form.style.fields.outerMargin
          rightMargin: form.style.fields.outerMargin
        }

        height: fieldContainer.shouldBeVisible ? childrenRect.height : 0

        Item {
          id: fieldLabelContainer

          height: fieldLabel.height + fieldValidationText.height + form.style.fields.sideMargin

          anchors {
            left: paddedEditorField.left
            right: paddedEditorField.right
            topMargin: form.style.fields.sideMargin
            bottomMargin: form.style.fields.sideMargin
          }

          Label {
            id: fieldLabel

            text: Name
            color: form.style.constraint.validColor
            leftPadding: form.style.fields.sideMargin
            font.pixelSize: form.style.fields.labelPixelSize
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            anchors.top: fieldLabelContainer.top
          }

          Label {
            id: fieldValidationText

            anchors {
              left: fieldLabelContainer.left
              right: fieldLabelContainer.right
              top: fieldLabel.bottom
              leftMargin: form.style.fields.sideMargin
            }

            text: ValidationMessage
            visible: ValidationMessage // show if there is something
            height: visible ? paintedHeight : 0
            wrapMode: Text.WordWrap
            opacity: visible ? 1 : 0
            color: ValidationStatus === FieldValidator.Warning ? form.style.constraint.descriptionColor : form.style.constraint.invalidColor
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter

            Behavior on height {
              NumberAnimation { duration: 100 }
            }

            Behavior on opacity {
              NumberAnimation { duration: 100 }
            }
          }
        }

        Item {
          id: placeholder
          height: childrenRect.height
          anchors {
            left: paddedEditorField.left
            right: rememberCheckboxContainer.left
            top: fieldLabelContainer.bottom
          }

          Loader {
            id: attributeEditorLoader

            height: childrenRect.height
            anchors { left: placeholder.left; right: placeholder.right }

            property var value: RawValue
            property bool valueIsNull: RawValueIsNull

            property var field: Field
            property var widget: EditorWidget
            property var config: EditorWidgetConfig

            property var homePath: form.project ? form.project.homePath : ""
            property var externalResourceHandler: form.externalResourceHandler

            property var customStyle: form.style
            property bool readOnly: form.state === "readOnly" || !AttributeEditable

            property var labelAlias: Name
            property var activeProject: form.project
            property var associatedRelation: Relation
            property var featurePair: form.controller.featureLayerPair

            property var formView: extraView //! passes StackView to editor, so that editors can show fullpage views (VR page, camera,..)

            active: widget !== 'Hidden'
            Keys.forwardTo: backHandler

            source: {
              if ( widget !== undefined )
                return __inputUtils.getEditorComponentSource( widget.toLowerCase(), config, field )
              else return ''
            }
          }

          Connections {
            target: attributeEditorLoader.item
            ignoreUnknownSignals: true

            function onEditorValueChanged( newValue, isNull ) {
              AttributeValue = isNull ? undefined : newValue
            }

            function onOpenLinkedFeature( linkedFeature ) {
              form.openLinkedFeature( linkedFeature )
            }

            function onCreateLinkedFeature( parentFeature, relation ) {
              let parentHasValidId = __inputUtils.isFeatureIdValid( parentFeature.feature.id )

              if ( parentHasValidId ) {
                // parent feature in this case already have valid id, so we can open new form
                form.createLinkedFeature( form.controller, relation )
              }
              else {
                // parent feature do not have a valid ID yet, we need to save it and acquire ID
                form.controller.acquireId()
                form.createLinkedFeature( form.controller, relation )
              }
            }
          }

          Connections {
            target: form.controller

            function onFeatureLayerPairChanged() {
              if ( attributeEditorLoader.item && attributeEditorLoader.item.featureLayerPairChanged )
              {
                attributeEditorLoader.item.featureLayerPairChanged()
              }
            }

            function onFormRecalculated() {
              if ( attributeEditorLoader.item && attributeEditorLoader.item.reload )
              {
                attributeEditorLoader.item.reload()
              }
            }
          }

          Connections {
            target: form
            ignoreUnknownSignals: true

            function onSaved() {
              if (attributeEditorLoader.item && typeof attributeEditorLoader.item.callbackOnSave === "function") {
                attributeEditorLoader.item.callbackOnSave()
              }
            }

            function onCanceled() {
              if (attributeEditorLoader.item && typeof attributeEditorLoader.item.callbackOnCancel === "function") {
                attributeEditorLoader.item.callbackOnCancel()
              }
            }
          }
        }

        Item {
          id: rememberCheckboxContainer
          visible: form.controller.rememberAttributesController.rememberValuesAllowed && form.state === "add" && EditorWidget !== "Hidden" && Type === FormItem.Field

          implicitWidth: visible ? 35 * __dp : 0
          implicitHeight: placeholder.height

          anchors {
            top: fieldLabelContainer.bottom
            right: paddedEditorField.right
          }

          CheckboxComponent {
            id: rememberCheckbox
            visible: rememberCheckboxContainer.visible
            baseColor: form.style.checkboxComponent.baseColor

            implicitWidth: 40 * __dp
            implicitHeight: width
            y: rememberCheckboxContainer.height/2 - rememberCheckbox.height/2
            x: (rememberCheckboxContainer.width + form.style.fields.outerMargin) / 7

            onCheckboxClicked: function( buttonState ) {
              RememberValue = buttonState
            }
            checked: RememberValue ? true : false
          }

          MouseArea {
            anchors.fill: rememberCheckboxContainer
            onClicked: rememberCheckbox.checkboxClicked( !rememberCheckbox.checkState )
          }
        }
      }
    }
  }

  Connections {
    target: Qt.inputMethod
    function onVisibleChanged() {
      Qt.inputMethod.commit()
    }
  }

  Connections {
    target: form.controller
    function onChangesCommited() {
      form.saved()
    }
    function onCommitFailed() {
      form.editingFailed()
    }
  }
}
