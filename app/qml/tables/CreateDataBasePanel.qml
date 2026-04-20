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
 * CreateDataBasePanel — página de creación de base de datos SQLite.
 * Sigue el patrón MMPage del resto de la app.
 */

MMPage {
  id: root

  // ── API pública ───────────────────────────────────────────────────────
  property var dbManager: null

  signal closed()
  signal databaseCreated(string dbName, string dbPath)

  // ── Cabecera ──────────────────────────────────────────────────────────
  pageHeader {
    title: qsTr("Crear Base de Datos")
    titleFont: __style.h3
    baseHeaderHeight: __style.row80
  }

  Keys.onReleased: function( event ) {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      root.closed()
    }
  }

  onBackClicked: function() {
    console.log("msj: Sale del control de DB")
    root.closed()
  }


  // ── Contenido principal ───────────────────────────────────────────────
  pageContent: MMScrollView {
    width: parent.width
    height: parent.height



    ColumnLayout {
      width: parent.width
      spacing: __style.spacing20

      // Campo: nombre de la BD
      MMTextInput {
        id: dbNameInput
        Layout.fillWidth: true
        title: qsTr("Nombre de la Base de Datos")
        placeholderText: qsTr("Ej: miproyecto")
      }

      // Campo: ubicación (opcional)
      MMTextInput {
        id: dbPathInput
        Layout.fillWidth: true
        title: qsTr("Ubicación (dejar vacío para ruta predeterminada)")
        placeholderText: qsTr("Ej: E:/MisDocumentos/")
      }

      // Mensaje de éxito
      Rectangle {
        id: successBox
        Layout.fillWidth: true
        implicitHeight: successText.implicitHeight + __style.margin16 * 2
        visible: successText.text !== ""
        color: __style.lightGreenColor
        radius: __style.radius12
        border.color: __style.grassColor
        border.width: __style.width1

        MMText {
          id: successText
          anchors { left: parent.left; right: parent.right; top: parent.top; margins: __style.margin16 }
          text: ""
          font: __style.p5
          color: __style.forestColor
          wrapMode: Text.WordWrap
        }
      }

      // Mensaje de error
      Rectangle {
        id: errorBox
        Layout.fillWidth: true
        implicitHeight: errorText.implicitHeight + __style.margin16 * 2
        visible: errorText.text !== ""
        color: __style.negativeLightColor
        radius: __style.radius12
        border.color: __style.grapeColor
        border.width: __style.width1

        MMText {
          id: errorText
          anchors { left: parent.left; right: parent.right; top: parent.top; margins: __style.margin16 }
          text: ""
          font: __style.p5
          color: __style.grapeColor
          wrapMode: Text.WordWrap
        }
      }

      Item { implicitHeight: __style.spacing20 }

      // Botones de acción
      RowLayout {
        Layout.fillWidth: true
        spacing: __style.spacing12

        MMButton {
          text: qsTr("Crear")
          Layout.fillWidth: true
          onClicked: createDatabase()
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

  // ── Timer de cierre automático tras éxito ─────────────────────────────
  Timer {
    id: successTimer
    interval: 2000
    onTriggered: root.closed()
  }

  // ── Lógica de creación ─────────────────────────────────────────────────
  function createDatabase() {
    if (dbNameInput.text.trim() === "") {
      errorText.text = qsTr("El nombre no puede estar vacío")
      successText.text = ""
      return
    }

    var dbPath = dbPathInput.text.trim()
    var dbName = dbNameInput.text.trim()

    if (dbPath === "") dbPath = "./"
    if (!dbPath.endsWith("/") && !dbPath.endsWith("\\")) dbPath += "/"

    var fullPath = dbPath + dbName + ".db"
    console.log("Intentando crear BD en: " + fullPath)

    if (dbManager && dbManager.initializeDatabase(fullPath)) {
      successText.text = qsTr("Base de datos creada en:\n") + dbManager.databasePath
      errorText.text = ""
      dbNameInput.text = ""
      dbPathInput.text = ""
      root.databaseCreated(dbName, dbManager.databasePath)
      successTimer.start()
    } else {
      errorText.text = qsTr("Error: ") + (dbManager ? dbManager.getLastError() : qsTr("DBManager no disponible"))
      successText.text = ""
    }
  }

  Component.onCompleted: {
    console.log("CreateDatabasePanel cargado")
  }
}
