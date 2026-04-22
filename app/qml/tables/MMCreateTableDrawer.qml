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

import mm 1.0 as MM

import "../components"
import "../inputs"
import "components"

/*
 * MMCreateTableDrawer — full-page component for creating a new table.
 * Migrated from MMDrawer to MMPage to support full-screen layout.
 * All communication with the backend is done via signals and properties.
 */

MMPage {
  id: root

  // ── Propiedades de entrada ────────────────────────────────────────────
  property string dbNameToShow: ""
  property string dbPathToShow: ""
  property string errorMessage: ""   // el Controller escribe aquí si falla

  // ── Señales de salida ─────────────────────────────────────────────────
  signal createTableRequested(string tableName, var fields)
  signal closed()

  // ── Cabecera ──────────────────────────────────────────────────────────
  pageHeader {
    title: qsTr("Crear Nueva Tabla")
    titleFont: __style.h3
    baseHeaderHeight: __style.row60
    backVisible: true
  }

  onBackClicked: root.closed()

  // ── Contenido ──────────────────────────────────────────────────────────
  pageContent: MMScrollView {
    width: parent.width
    height: parent.height

    ColumnLayout {
      width: parent.width
      spacing: __style.spacing12

      // Formulario visual
      MMCreateTableForm {
        id: uiForm
        Layout.fillWidth: true

        dbNameFieldText: root.dbNameToShow
        dbPathFieldText: root.dbPathToShow
        fieldsModel: fieldsListModel

        onShowDbInfoRequested: {
          dbInfoDrawer.open()
        }

        onAddFieldRequested:        fieldsListModel.addField()
        onMoveFieldUpRequested:     function(index) { fieldsListModel.moveField(index, index - 1) }
        onMoveFieldDownRequested:   function(index) { fieldsListModel.moveField(index, index + 1) }
        onRemoveFieldRequested:     function(index) { fieldsListModel.removeField(index) }
      }

      // Notificación de error (visible cuando errorMessage no está vacío)
      MMNotificationBox {
        id: errorNotification
        Layout.fillWidth: true
        visible: root.errorMessage !== ""
        type: MMNotificationBox.Types.Error
        title: qsTr("Error")
        description: root.errorMessage
      }

      // Botones de acción
      RowLayout {
        Layout.fillWidth: true
        spacing: __style.spacing12

        MMButton {
          text: qsTr("Aceptar")
          Layout.fillWidth: true
          onClicked: {
            if (validateInputs()) {
              root.createTableRequested(uiForm.tableNameText.trim(), fieldsListModel.getFieldsList())
            }
          }
        }

        MMButton {
          text: qsTr("Cancelar")
          type: MMButton.Types.Secondary
          Layout.fillWidth: true
          onClicked: root.closed()
        }
      }
    }
  }

  // ── Modelo de campos ─────────────────────────────────────────────────
  ListModel {
    id: fieldsListModel

    function addField()               { append({ fieldName: "", fieldType: "TEXT", fieldSize: "" }) }
    function removeField(index)       { remove(index) }
    function moveField(from, to)      { if (to >= 0 && to < count) move(from, to, 1) }

    function getFieldsList() {
      var fields = []
      for (var i = 0; i < count; i++) {
        fields.push({ name: get(i).fieldName, type: get(i).fieldType, size: get(i).fieldSize })
      }
      return fields
    }
  }

  // ── Drawer de información de BD ────────────────────────────────────────
  MMDatabaseInfoDrawer {
    id: dbInfoDrawer
    infoText: root.dbNameToShow + (root.dbPathToShow ? "\n" + root.dbPathToShow : "")
  }

  // ── Validación de formulario (lógica de UI pura) ──────────────────────
  function validateInputs() {
    if (uiForm.tableNameText.trim() === "") {
      uiForm.messageTextContent = qsTr("El nombre de la tabla no puede estar vacío")
      uiForm.messageType = "error"
      return false
    }

    if (!/^[a-zA-Z_][a-zA-Z0-9_]*$/.test(uiForm.tableNameText)) {
      uiForm.messageTextContent = qsTr("El nombre solo puede contener letras, números y guiones bajos")
      uiForm.messageType = "error"
      return false
    }

    if (fieldsListModel.count === 0) {
      uiForm.messageTextContent = qsTr("Debe agregar al menos un campo")
      uiForm.messageType = "error"
      return false
    }

    for (var i = 0; i < fieldsListModel.count; i++) {
      var field = fieldsListModel.get(i)
      if (field.fieldName.trim() === "") {
        uiForm.messageTextContent = qsTr("El campo %1 no tiene nombre").arg(i + 1)
        uiForm.messageType = "error"
        return false
      }
      if (!/^[a-zA-Z_][a-zA-Z0-9_]*$/.test(field.fieldName)) {
        uiForm.messageTextContent = qsTr("El campo '%1' tiene un nombre inválido").arg(field.fieldName)
        uiForm.messageType = "error"
        return false
      }
    }

    uiForm.messageTextContent = ""
    uiForm.messageType = "info"
    return true
  }

  // ── Inicialización ─────────────────────────────────────────────────────
  Component.onCompleted: {
    uiForm.messageTextContent = ""
    uiForm.messageType = "info"
    if (fieldsListModel.count === 0) fieldsListModel.addField()
  }
}
