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
 * MMCreateTableDrawer — full-page form to create a new SQLite table.
 * Layout (top → bottom, all scrollable):
 *   1. Info de BD (nombre + ubicación + botón Ver Info)
 *   2. Nombre de la tabla
 *   3. Definición de campos (lista + botón Agregar Campo)
 *   4. Mensaje de validación
 *   5. Botones Aceptar / Cancelar
 */

MMPage {
  id: root

  // ── Propiedades de entrada ────────────────────────────────────────────
  property string dbNameToShow: ""
  property string dbPathToShow: ""
  property string errorMessage: ""

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

  // ── Contenido: scroll vertical ────────────────────────────────────────
  pageContent: MMScrollView {
    width: parent.width
    height: parent.height

    ColumnLayout {
      width: parent.width
      spacing: __style.spacing12

      // ── 1. Info de BD ───────────────────────────────────────────────
      Rectangle {
        Layout.fillWidth: true
        implicitHeight: dbInfoCol.implicitHeight + __style.margin16 * 2
        color: __style.lightGreenColor
        radius: __style.radius12
        border.color: __style.greyColor
        border.width: __style.width1

        ColumnLayout {
          id: dbInfoCol
          anchors { left: parent.left; right: parent.right; top: parent.top; margins: __style.margin16 }
          spacing: __style.spacing10

          MMText {
            text: qsTr("Información de Base de Datos")
            font: __style.t3
            color: __style.forestColor
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
          }

          MMText { text: qsTr("Nombre de BD:"); font: __style.t4; color: __style.forestColor }

          Rectangle {
            Layout.fillWidth: true
            implicitHeight: __style.row50
            color: __style.polarColor
            radius: __style.radius12
            border.color: __style.greyColor
            border.width: __style.width1

            TextField {
              anchors.fill: parent
              leftPadding: __style.margin12; rightPadding: __style.margin12
              topPadding: 0; bottomPadding: 0
              readOnly: true
              text: root.dbNameToShow
              color: __style.nightColor
              font: __style.p5
              placeholderText: qsTr("Ej: miproyecto")
              placeholderTextColor: __style.darkGreyColor
              background: Item {}
            }
          }

          MMText { text: qsTr("Ubicación:"); font: __style.t4; color: __style.forestColor }

          Rectangle {
            Layout.fillWidth: true
            implicitHeight: __style.row50
            color: __style.polarColor
            radius: __style.radius12
            border.color: __style.greyColor
            border.width: __style.width1

            TextField {
              anchors.fill: parent
              leftPadding: __style.margin12; rightPadding: __style.margin12
              topPadding: 0; bottomPadding: 0
              readOnly: true
              text: root.dbPathToShow
              color: __style.nightColor
              font: __style.p5
              placeholderText: qsTr("Ruta de almacenamiento")
              placeholderTextColor: __style.darkGreyColor
              background: Item {}
            }
          }

          MMButton {
            text: qsTr("Ver Información Completa")
            type: MMButton.Types.Secondary
            size: MMButton.Sizes.Small
            Layout.alignment: Qt.AlignRight
            onClicked: dbInfoDrawer.open()
          }
        }
      }

      // ── 2. Nombre de la tabla ────────────────────────────────────────
      Rectangle {
        Layout.fillWidth: true
        implicitHeight: tableNameCol.implicitHeight + __style.margin16 * 2
        color: __style.lightGreenColor
        radius: __style.radius12
        border.color: __style.greyColor
        border.width: __style.width1

        ColumnLayout {
          id: tableNameCol
          anchors { left: parent.left; right: parent.right; top: parent.top; margins: __style.margin16 }
          spacing: __style.spacing10

          MMText { text: qsTr("Nombre de la Tabla"); font: __style.t3; color: __style.forestColor }

          MMText {
            text: qsTr("Solo letras, números y guiones bajos")
            font: __style.p6; color: __style.nightColor
            wrapMode: Text.WordWrap; Layout.fillWidth: true
          }

          MMTextInput {
            id: tableNameField
            Layout.fillWidth: true
            placeholderText: qsTr("Ej: usuarios, productos, clientes")
          }
        }
      }

      // ── 3. Definición de campos ──────────────────────────────────────
      Rectangle {
        Layout.fillWidth: true
        implicitHeight: fieldsCol.implicitHeight + __style.margin16 * 2
        color: __style.lightGreenColor
        radius: __style.radius12
        border.color: __style.greyColor
        border.width: __style.width1

        ColumnLayout {
          id: fieldsCol
          anchors { left: parent.left; right: parent.right; top: parent.top; margins: __style.margin16 }
          spacing: __style.spacing10

          MMText { text: qsTr("Definición de Campos"); font: __style.t3; color: __style.forestColor }

          // Lista de campos
          Repeater {
            id: fieldsRepeater
            model: fieldsListModel

            delegate: Rectangle {
              Layout.fillWidth: true
              implicitWidth: fieldsCol.width
              implicitHeight: fieldRow.implicitHeight + __style.margin8 * 2
              color: index % 2 === 0 ? __style.polarColor : __style.lightGreenColor
              radius: __style.radius8
              border.color: __style.greyColor
              border.width: __style.width1

              ColumnLayout {
                id: fieldRow
                anchors { left: parent.left; right: parent.right; top: parent.top; margins: __style.margin8 }
                spacing: __style.spacing5

                // Fila 1: nombre del campo + botón eliminar
                RowLayout {
                  Layout.fillWidth: true
                  spacing: __style.spacing10

                  Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: __style.row50
                    color: __style.polarColor
                    radius: __style.radius8
                    border.color: fieldNameInput.activeFocus ? __style.forestColor : __style.greyColor
                    border.width: fieldNameInput.activeFocus ? __style.width2 : __style.width1

                    TextField {
                      id: fieldNameInput
                      anchors.fill: parent
                      leftPadding: __style.margin12; rightPadding: __style.margin12
                      topPadding: 0; bottomPadding: 0
                      text: model.fieldName
                      color: __style.nightColor
                      font: __style.p5
                      placeholderText: qsTr("Nombre del campo")
                      placeholderTextColor: __style.darkGreyColor
                      background: Item {}
                      onTextEdited: model.fieldName = text
                    }
                  }

                  MMRoundButton {
                    iconSource: __style.closeIcon
                    onClicked: fieldsListModel.removeField(index)
                  }
                }

                // Fila 2: tipo + tamaño + mover
                RowLayout {
                  Layout.fillWidth: true
                  spacing: __style.spacing10

                  // Tipo
                  Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: __style.row50
                    color: __style.polarColor
                    radius: __style.radius8
                    border.color: __style.greyColor
                    border.width: __style.width1

                    ComboBox {
                      id: typeCombo
                      anchors.fill: parent
                      model: ["INT", "TEXT", "REAL", "DATE", "BOOLEAN", "BLOB"]
                      currentIndex: find(fieldType)
                      font: __style.p5
                      background: Item {}
                      contentItem: MMText {
                        text: typeCombo.displayText
                        font: __style.p5
                        color: __style.nightColor
                        leftPadding: __style.margin8
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                      }
                      onCurrentTextChanged: model.fieldType = currentText
                    }
                  }

                  // Tamaño (solo TEXT)
                  Rectangle {
                    implicitWidth: Math.max(60 * __dp, parent.width * 0.28)
                    implicitHeight: __style.row50
                    color: typeCombo.currentText === "TEXT" ? __style.polarColor : __style.mediumGreenColor
                    radius: __style.radius8
                    border.color: sizeInput.activeFocus ? __style.forestColor : __style.greyColor
                    border.width: sizeInput.activeFocus ? __style.width2 : __style.width1

                    TextField {
                      id: sizeInput
                      anchors.fill: parent
                      leftPadding: __style.margin8; rightPadding: __style.margin8
                      topPadding: 0; bottomPadding: 0
                      text: model.fieldSize
                      enabled: typeCombo.currentText === "TEXT"
                      color: enabled ? __style.nightColor : __style.darkGreenColor
                      font: __style.p5
                      inputMethodHints: Qt.ImhDigitsOnly
                      placeholderText: qsTr("255")
                      placeholderTextColor: __style.darkGreyColor
                      background: Item {}
                      onTextEdited: model.fieldSize = text
                    }
                  }

                  // Mover arriba / abajo
                  MMRoundButton {
                    iconSource: __style.arrowUpIcon
                    enabled: index > 0
                    onClicked: fieldsListModel.moveField(index, index - 1)
                  }
                  MMRoundButton {
                    iconSource: __style.arrowDownIcon
                    enabled: index < fieldsRepeater.count - 1
                    onClicked: fieldsListModel.moveField(index, index + 1)
                  }
                }
              }
            }
          }

          // Botón agregar campo
          MMButton {
            text: qsTr("Agregar Campo")
            iconSourceLeft: __style.addIcon
            Layout.fillWidth: true
            onClicked: fieldsListModel.addField()
          }
        }
      }

      // ── 4. Error del controller ──────────────────────────────────────
      MMNotificationBox {
        Layout.fillWidth: true
        visible: root.errorMessage !== ""
        type: MMNotificationBox.Types.Error
        title: qsTr("Error")
        description: root.errorMessage
      }

      // ── 5. Mensaje de validación de UI ───────────────────────────────
      Rectangle {
        Layout.fillWidth: true
        implicitHeight: validationText.implicitHeight + __style.margin16 * 2
        visible: validationText.text !== ""
        color: validationIsError ? __style.negativeLightColor : __style.lightGreenColor
        radius: __style.radius12
        border.color: validationIsError ? __style.grapeColor : __style.grassColor
        border.width: __style.width1

        property bool validationIsError: validationText.color === __style.grapeColor

        MMText {
          id: validationText
          anchors { left: parent.left; right: parent.right; top: parent.top; margins: __style.margin16 }
          font: __style.p5
          color: __style.forestColor
          wrapMode: Text.WordWrap
        }
      }

      // ── 6. Botones Aceptar / Cancelar ────────────────────────────────
      RowLayout {
        Layout.fillWidth: true
        spacing: __style.spacing12

        MMButton {
          text: qsTr("Aceptar")
          Layout.fillWidth: true
          onClicked: {
            if (validateInputs()) {
              root.createTableRequested(tableNameField.text.trim(), fieldsListModel.getFieldsList())
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

      Item { implicitHeight: __style.spacing20 }
    }
  }

  // ── Modelo de campos ──────────────────────────────────────────────────
  ListModel {
    id: fieldsListModel

    function addField()          { append({ fieldName: "", fieldType: "TEXT", fieldSize: "" }) }
    function removeField(index)  { remove(index) }
    function moveField(from, to) { if (to >= 0 && to < count) move(from, to, 1) }

    function getFieldsList() {
      var fields = []
      for (var i = 0; i < count; i++)
        fields.push({ name: get(i).fieldName, type: get(i).fieldType, size: get(i).fieldSize })
      return fields
    }
  }

  // ── Drawer de información de BD ───────────────────────────────────────
  MMDatabaseInfoDrawer {
    id: dbInfoDrawer
    infoText: root.dbNameToShow + (root.dbPathToShow ? "\n" + root.dbPathToShow : "")
  }

  // ── Validación ────────────────────────────────────────────────────────
  function validateInputs() {
    var name = tableNameField.text.trim()

    if (name === "") {
      showValidation(qsTr("El nombre de la tabla no puede estar vacío"), true)
      return false
    }
    if (!/^[a-zA-Z_][a-zA-Z0-9_]*$/.test(name)) {
      showValidation(qsTr("El nombre solo puede contener letras, números y guiones bajos"), true)
      return false
    }
    if (fieldsListModel.count === 0) {
      showValidation(qsTr("Debe agregar al menos un campo"), true)
      return false
    }
    for (var i = 0; i < fieldsListModel.count; i++) {
      var f = fieldsListModel.get(i)
      if (f.fieldName.trim() === "") {
        showValidation(qsTr("El campo %1 no tiene nombre").arg(i + 1), true)
        return false
      }
      if (!/^[a-zA-Z_][a-zA-Z0-9_]*$/.test(f.fieldName)) {
        showValidation(qsTr("El campo '%1' tiene un nombre inválido").arg(f.fieldName), true)
        return false
      }
    }
    showValidation("", false)
    return true
  }

  function showValidation(msg, isError) {
    validationText.text = msg
    validationText.color = isError ? __style.grapeColor : __style.forestColor
  }

  // ── Inicialización ────────────────────────────────────────────────────
  Component.onCompleted: {
    if (fieldsListModel.count === 0) fieldsListModel.addField()
  }
}
