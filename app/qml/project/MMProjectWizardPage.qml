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

import mm 1.0 as MM

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
          id: fieldDelegate

          width: ListView.view.width - ListView.view.scrollBarWidth

          // find current index in the model
          comboboxField.comboboxModel: typesmodel
          comboboxField.valueRole: "type"

          comboboxField.onCurrentIndexChanged: {
            WidgetType = typesmodel.get(comboboxField.currentIndex)?.type ?? ""
          }

          onAttrNameChanged: ( attrname ) => AttributeName = attrname
          onRemoveClicked: () => fieldsModel.removeField( index )

          Component.onCompleted: {
            // assign initial values without binding
            attrname = AttributeName
            comboboxField.currentIndex = comboboxField.indexFromValue( WidgetType )
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
            __projectWizard.createProject(projectNameField.text, fieldsModel )
          }
        }
      }
    }
  }

  MM.FieldsModel {
    id: fieldsModel

    onNotifyError: function( message ) {
      __notificationModel.addError( message )
    }
  }

  ListModel {
    id: typesmodel

    ListElement { text: "Text"; type: "TextEdit" }
    ListElement { text: "Date&time"; type: "DateTime" }
    ListElement { text: "Number"; type: "Range" }
    ListElement { text: "Checkbox"; type: "CheckBox" }
    ListElement { text: "Photo"; type: "ExternalResource" }
  }
}
