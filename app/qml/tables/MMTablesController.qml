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

import "../components"

/*
 * MMTablesController — controlador del módulo de tablas.
 * Es el único componente que conoce dbManager.
 * Gestiona la navegación con un StackView interno y conecta señales
 * de las Pages/Drawers con llamadas al backend.
 */

Item {
  id: root

  // ── API pública ───────────────────────────────────────────────────────
  signal closed()

  property var dbManager: null

  // ── StackView de navegación ───────────────────────────────────────────
  StackView {
    id: stackView
    anchors.fill: parent

    Component.onCompleted: {
      stackView.push(databaseManagerPageComp, {}, StackView.Immediate)
    }
  }

  // ── Página principal: gestor de BD ────────────────────────────────────
  Component {
    id: databaseManagerPageComp

    MMDatabaseManagerPage {
      tableModel:   root.dbManager ? root.dbManager.tableModel : null
      tableList:    root.dbManager ? root.dbManager.tableList : []
      currentTable: root.dbManager ? root.dbManager.currentTable : ""
      rowCount:     root.dbManager ? root.dbManager.getRowCount() : 0

      onTableSelected:          function(name) { root.dbManager.setCurrentTable(name) }
      onAddRowRequested:        function() { root.dbManager.addRow() }
      onRemoveRowRequested:     function(i) { root.dbManager.removeRow(i) }
      onSubmitChangesRequested: function() {
        if (root.dbManager.submitChanges()) {
          viewStateSuccess()
        } else {
          viewStateError()
        }
      }
      onRevertChangesRequested: function() { root.dbManager.revertChanges() }
      onFilterRequested:        function(text) { root.dbManager.filterTable(text) }
      onClearFilterRequested:   function() { root.dbManager.clearFilter() }
      onCreateTableRequested:   function() { stackView.push(createTableDrawerComp, {}, StackView.PushTransition) }
      onCreateDatabaseRequested: function() { stackView.push(createDatabasePageComp, {}, StackView.PushTransition) }
      onBackClicked:            function() { root.closed() }
    }
  }

  // ── Página: crear base de datos ───────────────────────────────────────
  Component {
    id: createDatabasePageComp

    MMCreateDatabasePage {
      onCreateDatabaseRequested: function(name, path) {
        if (name.trim() === "") {
          errorMessage = qsTr("El nombre no puede estar vacío")
          return
        }

        var dbPath = path.trim()
        if (dbPath === "") dbPath = "./"
        if (!dbPath.endsWith("/") && !dbPath.endsWith("\\")) dbPath += "/"

        var fullPath = dbPath + name.trim() + ".db"
        console.log("Intentando crear BD en: " + fullPath)

        if (root.dbManager && root.dbManager.initializeDatabase(fullPath)) {
          closeAfterSuccessTimer.start()
        } else {
          errorMessage = qsTr("Error: ") + (root.dbManager ? root.dbManager.getLastError() : qsTr("DBManager no disponible"))
        }
      }
      onBackClicked: function() { stackView.pop(StackView.PopTransition) }
    }
  }

  // ── Drawer: crear tabla ───────────────────────────────────────────────
  Component {
    id: createTableDrawerComp

    MMCreateTableDrawer {
      dbNameToShow: root.dbManager ? root.dbManager.databaseName : ""
      dbPathToShow: root.dbManager ? root.dbManager.databasePath : ""

      onCreateTableRequested: function(tableName, fields) {
        if (!root.dbManager) {
          errorMessage = qsTr("DBManager no está configurado")
          return
        }

        if (root.dbManager.createTable(tableName, fields)) {
          stackView.pop(StackView.PopTransition)
        } else {
          errorMessage = qsTr("Error: ") + root.dbManager.getLastError()
        }
      }
      onClosed: function() { stackView.pop(StackView.PopTransition) }
    }
  }

  // ── Timer de cierre automático tras crear BD con éxito ────────────────
  Timer {
    id: closeAfterSuccessTimer
    interval: 1500
    onTriggered: stackView.pop(StackView.PopTransition)
  }

  // ── Conexiones con el backend ──────────────────────────────────────────
  Connections {
    target: root.dbManager

    function onErrorOccurred(errorMessage) {
      console.log("MMTablesController: error en dbManager: " + errorMessage)
    }

    function onTableModelChanged() {
      console.log("MMTablesController: tableModel actualizado")
    }

    function onDataChanged() {
      console.log("MMTablesController: datos actualizados")
    }
  }

  // ── Helpers de estado ─────────────────────────────────────────────────
  function viewStateSuccess() {
    console.log("MMTablesController: operación exitosa")
  }

  function viewStateError() {
    console.log("MMTablesController: operación con error")
  }
}
