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

/*
 * CreateTableDialog — bottom-sheet drawer para crear una nueva tabla.
 * Reemplaza el Qt Dialog por MMDrawer siguiendo el patrón de la app.
 */

MMDrawer {
  id: root

  // ── API pública ───────────────────────────────────────────────────────
  property var dbManager: null
  property string dbNameToShow: ""
  property string dbPathToShow: ""

  signal tableCreationRequested(string tableName, var fields)

  // ── Cabecera del drawer ───────────────────────────────────────────────
  drawerHeader.title: qsTr("Crear Nueva Tabla")

  // ── Contenido ──────────────────────────────────────────────────────────
  drawerContent: ColumnLayout {
    width: parent.width
    spacing: __style.spacing12

    // Formulario visual (CreateTableForm refactorizado)
    CreateTableForm {
      id: uiForm
      Layout.fillWidth: true

      dbNameFieldText: root.dbNameToShow
      dbPathFieldText: root.dbPathToShow
      fieldsModel: fieldsListModel

      onShowDbInfoRequested: {
        if (root.dbManager) {
          dbInfoDrawer.infoText = root.dbManager.getDatabaseInfo()
          dbInfoDrawer.open()
        }
      }

      onAddFieldRequested:        fieldsListModel.addField()
      onMoveFieldUpRequested:     function(index) { fieldsListModel.moveField(index, index - 1) }
      onMoveFieldDownRequested:   function(index) { fieldsListModel.moveField(index, index + 1) }
      onRemoveFieldRequested:     function(index) { fieldsListModel.removeField(index) }
    }

    // Caja de mensajes de validación / resultado
    //MMNotificationBox { // checar
    //  id: messageBox
    //  Layout.fillWidth: true
    //  visible: messageBox.description !== ""
    //  type: internal.isError ? MMNotificationBox.Types.Error : MMNotificationBox.Types.Warning
   // }

    // Botones de acción
    RowLayout {
      Layout.fillWidth: true
      spacing: __style.spacing12

      MMButton {
        text: qsTr("Aceptar")
        Layout.fillWidth: true
        onClicked: {
          if (validateInputs()) {
            createTableAction()
          }
        }
      }

      MMButton {
        text: qsTr("Cancelar")
        type: MMButton.Types.Secondary
        Layout.fillWidth: true
        onClicked: root.close()
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

  // ── Lógica de validación y creación ──────────────────────────────────
  function showMessage(message, isErr) {
    messageBox.description = message
    internal.isError = isErr
  }

  function clearMessage() {
    messageBox.description = ""
    internal.isError = false
  }

  function validateInputs() {
    clearMessage()

    if (uiForm.tableNameText.trim() === "") {
      showMessage(qsTr("El nombre de la tabla no puede estar vacío"), true)
      return false
    }

    if (!/^[a-zA-Z_][a-zA-Z0-9_]*$/.test(uiForm.tableNameText)) {
      showMessage(qsTr("El nombre solo puede contener letras, números y guiones bajos"), true)
      return false
    }

    if (root.dbManager && root.dbManager.tableExists(uiForm.tableNameText)) {
      showMessage(qsTr("La tabla '%1' ya existe").arg(uiForm.tableNameText), true)
      return false
    }

    if (fieldsListModel.count === 0) {
      showMessage(qsTr("Debe agregar al menos un campo"), true)
      return false
    }

    for (var i = 0; i < fieldsListModel.count; i++) {
      var field = fieldsListModel.get(i)
      if (field.fieldName.trim() === "") {
        showMessage(qsTr("El campo %1 no tiene nombre").arg(i + 1), true)
        return false
      }
      if (!/^[a-zA-Z_][a-zA-Z0-9_]*$/.test(field.fieldName)) {
        showMessage(qsTr("El campo '%1' tiene un nombre inválido").arg(field.fieldName), true)
        return false
      }
    }

    return true
  }

  function createTableAction() {
    if (!root.dbManager) {
      showMessage(qsTr("DBManager no está configurado"), true)
      return
    }

    var fields = fieldsListModel.getFieldsList()
    var tableName = uiForm.tableNameText.trim()

    if (root.dbManager.createTable(tableName, fields)) {
      showMessage(qsTr("Tabla '%1' creada exitosamente").arg(tableName), false)
      closeTimer.start()
    } else {
      showMessage(qsTr("Error: ") + root.dbManager.getLastError(), true)
    }
  }

  // ── Timer de cierre automático tras éxito ────────────────────────────
  Timer {
    id: closeTimer
    interval: 2000
    onTriggered: root.close()
  }

  // ── Drawer de información de BD ────────────────────────────────────────
  MMDrawer {
    id: dbInfoDrawer
    property string infoText: ""

    drawerHeader.title: qsTr("Información de Base de Datos")

    drawerContent: MMScrollView {
      width: parent.width
      height: Math.min(400 * __dp, (ApplicationWindow.window?.height ?? 600) * 0.5)

      MMText {
        width: parent.width
        text: dbInfoDrawer.infoText
        font: __style.p6
        color: __style.nightColor
        wrapMode: Text.WordWrap
      }
    }
  }

  // ── Estado interno ────────────────────────────────────────────────────
  QtObject {
    id: internal
    property bool isError: false
  }

  // ── Inicialización ─────────────────────────────────────────────────────
  onOpened: clearMessage()

  Component.onCompleted: fieldsListModel.addField()
}
