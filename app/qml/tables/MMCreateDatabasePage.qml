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
 * MMCreateDatabasePage — página de creación de base de datos SQLite.
 * Sigue el patrón MMPage: solo vista, sin lógica de negocio.
 * Toda la comunicación con el backend se hace mediante señales y propiedades.
 */

MMPage {
  id: root

  // ── Propiedades de entrada ────────────────────────────────────────────
  property string errorMessage: ""   // el Controller escribe aquí si falla

  // ── Señales de salida ─────────────────────────────────────────────────
  signal createDatabaseRequested(string name, string path)
  signal backClicked()

  // ── Cabecera ──────────────────────────────────────────────────────────
  pageHeader {
    title: qsTr("Crear Base de Datos")
    titleFont: __style.h3
    baseHeaderHeight: __style.row80
    backVisible: true
  }

  Keys.onReleased: function( event ) {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      root.backClicked()
    }
  }

  onBackClicked: root.backClicked()

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

      // Notificación de error (visible cuando errorMessage no está vacío)
      MMNotificationBox {
        Layout.fillWidth: true
        visible: root.errorMessage !== ""
        type: MMNotificationBox.Types.Error
        title: qsTr("Error")
        description: root.errorMessage
      }

      Item { implicitHeight: __style.spacing20 }

      // Botones de acción
      RowLayout {
        Layout.fillWidth: true
        spacing: __style.spacing12

        MMButton {
          text: qsTr("Crear")
          Layout.fillWidth: true
          onClicked: {
            if (dbNameInput.text.trim() === "") {
              // Validación básica: emitir con nombre vacío para que el Controller lo maneje
              root.createDatabaseRequested("", dbPathInput.text.trim())
            } else {
              root.createDatabaseRequested(dbNameInput.text.trim(), dbPathInput.text.trim())
            }
          }
        }

        MMButton {
          text: qsTr("Cancelar")
          type: MMButton.Types.Secondary
          Layout.fillWidth: true
          onClicked: root.backClicked()
        }
      }
    }
  }

  Component.onCompleted: {
    console.log("MMCreateDatabasePage cargado")
  }
}
