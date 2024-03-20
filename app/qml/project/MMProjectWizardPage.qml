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

import "./components"
import "../components"
import "../inputs"

MMPage {
  id: root

  property real rowHeight: 50 * __dp
  property ListModel widgetsModel: {
    var model = Qt.createQmlObject('import QtQuick; ListModel {}', root);
    var types = fieldsModel.supportedTypes()
    for (var prop in types) {
      model.append({ WidgetName: types[prop], WidgetType: prop });
    }
    return model;
  }

  property var widgetType2WidgetName: {
    var ret = {}
    var types = fieldsModel.supportedTypes()
    for (var prop in types) {
      ret[prop] = types[prop]
    }
    return ret;
  }

  MM.FieldsModel {
    id: fieldsModel
    onNotifyError: function( message ) {
      __notificationModel.addError( message )
    }
    Component.onCompleted: fieldsModel.initModel()
  }

  pageHeader.title: qsTr("Create Project")

  pageContent: Item {

    width: parent.width
    height: parent.height

    Column {
      id: contentLayout

      width: parent.width
      height: parent.height

      spacing: 0

      MMListSpacer { height: __style.margin20 }

      MMTextInput {
        id: projectNameField

        title: qsTr("Project name")
        width: parent.width
      }

      MMListSpacer { height: __style.margin20 }

      MMText {
        id: attributesLabel

        height: root.rowheight
        width: parent.width
        text: qsTr("Fields")
        color: __style.nightColor
        font: __style.p6
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
      }

      ListView {
        id: fieldList

        model: fieldsModel
        width: parent.width
        height: parent.height - projectNameField.height - __style.margin20 - projectNameField.height
        clip: true
        spacing: __style.margin20

        delegate: MMProjectWizardDelegate {
          height: root.rowHeight
          width: ListView.view.width
          widgetList: root.widgetsModel
          widgetType2WidgetName: root.widgetType2WidgetName
          onRemoveClicked: function( index ) {
            fieldsModel.removeField(index)
          }
        }

        footer: MMButton {
          id: addButton

          width: ListView.view.width
          height: root.rowHeight
          anchors.horizontalCenter: parent.horizontalCenter

          text: qsTr( "Add field" )

          type: MMButton.Types.Tertiary

          iconSourceRight: __style.addIcon
          topPadding: __style.margin20

          onClicked: {
            fieldsModel.addField("", "TextEdit")
            if (fieldList.visible) {
              fieldList.positionViewAtEnd()
            }
          }
        }
      }
    }
  }

  footer: MMToolbar {
    id: toolbar

    model: ObjectModel {
      MMToolbarLongButton {
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
}
