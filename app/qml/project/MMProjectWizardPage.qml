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
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import QtQuick.Dialogs

import lc 1.0
import "."
import "../components"
import "../inputs"

Item {
  id: projectWizardPanel

  signal backClicked

  property real rowHeight: 50 * __dp

  property ListModel widgetsModel: ListModel {}

  //! Inits widgetsModel data just after its created, but before Component.complete is emitted (for both model or components where its used)
  property bool isWidgetModelReady: {
    var types = fieldsModel.supportedTypes()
    for (var prop in types) {
      // TODO - refactor, it is not  FeatureId and  FeatureTitle, but MMDropdownDrawer assumes it!
      // see ComboBox  - textRole: "display" and valueRole: "widget"
      projectWizardPanel.widgetsModel.append({ "FeatureTitle": types[prop], "FeatureId": prop })
    }

    true
  }

  FieldsModel {
    id: fieldsModel
    onNotify: function( message ) {
      __inputUtils.showNotification(message)
    }
    Component.onCompleted: fieldsModel.initModel()
  }

  // background
  Rectangle {
    width: parent.width
    height: parent.height
    color: __style.lightGreenColor
  }

  MMHeader {
    id: header
    width: projectWizardPanel.width
    color: __style.lightGreenColor
    title: qsTr("Create Project")

    onBackClicked: {
      projectWizardPanel.backClicked()
    }
  }

  Item {
    height: projectWizardPanel.height - header.height - toolbar.height
    width: projectWizardPanel.width
    y: header.height

    ColumnLayout {
      id: contentLayout
      spacing: 10 * __dp
      anchors.fill: parent
      anchors.leftMargin: __style.pageMargins
      anchors.rightMargin: __style.pageMargins

      MMTextInput {
        id: projectNameField
        title: qsTr("Project name")
        height: projectWizardPanel.rowheight
        width: parent.width
        Layout.fillWidth: true
        Layout.preferredHeight: projectWizardPanel.rowHeight
      }

      Label {
        id: attributesLabel
        height: projectWizardPanel.rowheight
        width: parent.width
        text: qsTr("Fields")
        color: __style.nightColor
        font: __style.p6
        Layout.preferredHeight: projectWizardPanel.rowHeight
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
      }

      ListView {
        id: fieldList
        model: fieldsModel
        width: parent.width
        Layout.fillWidth: true
        Layout.fillHeight: true
        clip: true
        spacing: 10 * __dp

        delegate: MMProjectWizardFieldRow {
          height: projectWizardPanel.rowHeight
          width: contentLayout.width
          widgetList: projectWizardPanel.widgetsModel
          onRemoveClicked: function( index ) {
            fieldsModel.removeField(index)
          }
        }

        footer: MMLink {
              text: qsTr("Add field")
              anchors.centerIn: parent
              rightIcon: __style.addIcon
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

  // footer toolbar
  MMToolbar {
    id: toolbar
    width: parent.width
    anchors.bottom: parent.bottom

    model: ObjectModel {
      MMToolbarLongButton {
        text: qsTr("Create project");
        iconSource: __style.doneCircleIcon;
        iconColor: toolbar.color
        onClicked: {
          if (!projectNameField.text) {
            __inputUtils.showNotification(qsTr("Empty project name"))
          } else {
            __projectWizard.createProject(projectNameField.text, fieldsModel )
          }
        }
      }
    }
  }
}
