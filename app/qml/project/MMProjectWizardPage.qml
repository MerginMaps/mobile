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

import MMInput

import "./components" as MMProjectComponents
import "../components" as MMComponents
import "../inputs" as MMInputs

MMComponents.MMPage {
  id: root

  pageHeader.title: qsTr("Create Project")

  pageBottomMargin: 0

  pageContent: Item {

    width: parent.width
    height: parent.height

    Column {
      id: contentLayout

      width: parent.width
      height: parent.height

      spacing: 0

      MMComponents.MMListSpacer { height: __style.margin20 }

      MMInputs.MMTextInput {
        id: projectNameField

        title: qsTr("Project name")
        width: parent.width
      }

      MMComponents.MMListSpacer { height: __style.margin20 }

      MMComponents.MMText {
        id: attributesLabel

        width: parent.width
        text: qsTr("Fields")
        color: __style.nightColor
        font: __style.p6
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
      }

      MMComponents.MMListView {
        id: fieldList

        model: fieldsModel
        width: parent.width
        height: parent.height - __style.margin20 - projectNameField.height - __style.margin20 - attributesLabel.height
        clip: true
        spacing: __style.margin20

        delegate: MMProjectComponents.MMProjectWizardDelegate {
          required property string widgetType
          required property string attributeName

          width: ListView.view.width

          // find current index in the model
          comboboxField.comboboxModel: typesmodel
          comboboxField.valueRole: "type"

          comboboxField.onCurrentIndexChanged: {
            widgetType = typesmodel.get( comboboxField.currentIndex )?.type ?? ""
          }

          onAttrNameChanged: ( attrName ) => attributeName = attrName
          onRemoveClicked: () => fieldsModel.removeField( index )

          Component.onCompleted: {
            // assign initial values without binding
            attrname = attributeName
            comboboxField.currentIndex = comboboxField.indexFromValue( widgetType )
          }
        }

        footer: MMComponents.MMButton {
          id: addButton

          width: ListView.view.width
          height: implicitHeight + topPadding

          text: qsTr( "Add field" )

          type: MMComponents.MMButton.Tertiary

          iconSourceRight: __style.addIcon
          topPadding: __style.margin20

          onClicked: {
            fieldsModel.addField("", "TextEdit")

            if ( fieldList.visible ) {
              fieldList.positionViewAtEnd()
            }
          }
        }
      }
    }
  }

  footer: MMComponents.MMToolbar {
    id: toolbar

    model: ObjectModel {
      MMComponents.MMToolbarButton {
        text: qsTr("Create project");
        iconSource: __style.doneCircleIcon
        iconColor: toolbar.color
        onClicked: {
          if (!projectNameField.text) {
            __notificationModel.addWarning( qsTr("Empty project name") )
          } else {
            __projectWizard.createProject( projectNameField.text, fieldsModel )
          }
        }
      }
    }
  }

  FieldsModel {
    id: fieldsModel

    onNotifyError: function( message ) {
      __notificationModel.addError( message )
    }
  }

  ListModel {
    id: typesmodel

    ListElement { text: qsTr( "Text" ); type: "TextEdit" }
    ListElement { text: qsTr( "Date & Time" ); type: "DateTime" }
    ListElement { text: qsTr( "Number" ); type: "Range" }
    ListElement { text: qsTr( "Checkbox" ); type: "CheckBox" }
    ListElement { text: qsTr( "Photo" ); type: "ExternalResource" }
  }
}
