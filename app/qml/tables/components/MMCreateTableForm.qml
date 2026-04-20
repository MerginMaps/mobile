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
import QtQuick.Controls.Basic
import QtQuick.Layouts

import mm 1.0 as MM

import "../../components"
import "../../inputs"

/*
 * MMCreateTableForm — formulario visual para definir campos de una tabla.
 * Componente de solo presentación; toda la lógica vive en MMCreateTableDrawer.
 * Usa el design-system (__style) en lugar de colores y fuentes hardcodeados.
 */

Item {
  id: root

  // ── Alias hacia los campos internos ──────────────────────────────────
  property alias dbNameFieldText:    dbNameField.text
  property alias dbPathFieldText:    dbPathField.text
  property alias tableNameText:      tableNameField.text
  property alias fieldsModel:        fieldsRepeater.model
  property alias messageTextContent: messageText.text
  property string messageType: "info"   // "info" | "error"

  // ── Señales hacia la lógica ───────────────────────────────────────────
  signal showDbInfoRequested()
  signal addFieldRequested()
  signal moveFieldUpRequested(int index)
  signal moveFieldDownRequested(int index)
  signal removeFieldRequested(int index)

  // ── Layout raíz ───────────────────────────────────────────────────────
  ColumnLayout {
    anchors.fill: parent
    spacing: __style.spacing12

    // ── SECCIÓN 1: Info de la BD ──────────────────────────────────────
    Rectangle {
      Layout.fillWidth: true
      implicitHeight: dbInfoColumn.implicitHeight + __style.margin20 * 2
      color: __style.lightGreenColor
      radius: __style.radius12
      border.color: __style.greyColor
      border.width: __style.width1

      ColumnLayout {
        id: dbInfoColumn
        anchors { top: parent.top; left: parent.left; right: parent.right; margins: __style.margin16 }
        spacing: __style.spacing12

        MMText {
          text: qsTr("Información de Base de Datos")
          font: __style.t3
          color: __style.forestColor
        }

        GridLayout {
          Layout.fillWidth: true
          columns: width >= 480 * __dp ? 2 : 1
          rowSpacing: __style.spacing10
          columnSpacing: __style.spacing12

          // Nombre BD (solo lectura)
          ColumnLayout {
            Layout.fillWidth: true
            spacing: __style.spacing5

            MMText { text: qsTr("Nombre de BD:"); font: __style.t4; color: __style.forestColor }

            Rectangle {
              Layout.fillWidth: true
              implicitHeight: __style.row50
              color: __style.polarColor
              radius: __style.radius12
              border.color: __style.greyColor
              border.width: __style.width1

              TextField {
                id: dbNameField
                anchors.fill: parent
                leftPadding: __style.margin12
                rightPadding: __style.margin12
                topPadding: 0
                bottomPadding: 0
                readOnly: true
                color: __style.nightColor
                font: __style.p5
                placeholderText: qsTr("Ej: miproyecto")
                placeholderTextColor: __style.darkGreyColor
                background: Item {}
              }
            }
          }

          // Ubicación BD (solo lectura)
          ColumnLayout {
            Layout.fillWidth: true
            spacing: __style.spacing5

            MMText { text: qsTr("Ubicación:"); font: __style.t4; color: __style.forestColor }

            Rectangle {
              Layout.fillWidth: true
              implicitHeight: __style.row50
              color: __style.polarColor
              radius: __style.radius12
              border.color: __style.greyColor
              border.width: __style.width1

              TextField {
                id: dbPathField
                anchors.fill: parent
                leftPadding: __style.margin12
                rightPadding: __style.margin12
                topPadding: 0
                bottomPadding: 0
                readOnly: true
                color: __style.nightColor
                font: __style.p5
                placeholderText: qsTr("Ruta de almacenamiento")
                placeholderTextColor: __style.darkGreyColor
                background: Item {}
              }
            }
          }
        }

        MMButton {
          text: qsTr("Ver Información Completa")
          type: MMButton.Types.Secondary
          size: MMButton.Sizes.Small
          Layout.alignment: Qt.AlignRight
          onClicked: root.showDbInfoRequested()
        }
      }
    }

    // ── SECCIÓN 2: Nombre de la tabla ─────────────────────────────────
    Rectangle {
      Layout.fillWidth: true
      implicitHeight: tableNameColumn.implicitHeight + __style.margin20 * 2
      color: __style.lightGreenColor
      radius: __style.radius12
      border.color: __style.greyColor
      border.width: __style.width1

      ColumnLayout {
        id: tableNameColumn
        anchors { top: parent.top; left: parent.left; right: parent.right; margins: __style.margin16 }
        spacing: __style.spacing10

        MMText { text: qsTr("Nombre de la Tabla"); font: __style.t3; color: __style.forestColor }

        MMText {
          text: qsTr("Solo letras, números y guiones bajos")
          font: __style.p6
          color: __style.nightColor
          Layout.fillWidth: true
        }

        MMTextInput {
          id: tableNameField
          Layout.fillWidth: true
          placeholderText: qsTr("Ej: usuarios, productos, clientes")
        }
      }
    }

    // ── SECCIÓN 3: Definición de campos ───────────────────────────────
    Rectangle {
      Layout.fillWidth: true
      Layout.fillHeight: true
      color: __style.lightGreenColor
      radius: __style.radius12
      border.color: __style.greyColor
      border.width: __style.width1

      ColumnLayout {
        anchors.fill: parent
        anchors.margins: __style.margin16
        spacing: __style.spacing10

        MMText { text: qsTr("Definición de Campos"); font: __style.t3; color: __style.forestColor }

        // Cabecera de columnas (solo escritorio)
        Rectangle {
          Layout.fillWidth: true
          implicitHeight: __style.row40
          color: __style.forestColor
          radius: __style.radius8
          visible: root.width >= 480 * __dp

          RowLayout {
            anchors.fill: parent
            anchors.leftMargin: __style.margin12
            anchors.rightMargin: __style.margin12
            spacing: __style.spacing10

            MMText { text: qsTr("Nombre del Campo"); font: __style.t4; color: __style.polarColor; Layout.fillWidth: true }
            MMText { text: qsTr("Tipo");             font: __style.t4; color: __style.polarColor; Layout.preferredWidth: 100 * __dp }
            MMText { text: qsTr("Tamaño");           font: __style.t4; color: __style.polarColor; Layout.preferredWidth: 90 * __dp }
            MMText { text: qsTr("Acciones");         font: __style.t4; color: __style.polarColor; Layout.preferredWidth: 100 * __dp }
          }
        }

        // Lista de campos con scroll
        ScrollView {
          Layout.fillWidth: true
          Layout.fillHeight: true
          clip: true

          ColumnLayout {
            width: root.width - __style.margin16 * 2
            spacing: __style.spacing5

            Repeater {
              id: fieldsRepeater

              delegate: Rectangle {
                Layout.fillWidth: true
                implicitHeight: root.width < 480 * __dp
                                ? (__style.row50 * 2 + __style.spacing10 + __style.margin16 * 2)
                                : __style.row60
                color: index % 2 === 0 ? __style.polarColor : __style.lightGreenColor
                radius: __style.radius8
                border.color: __style.greyColor
                border.width: __style.width1

                // ── Escritorio: fila única ──────────────────────────
                RowLayout {
                  anchors.fill: parent
                  anchors.margins: __style.margin8
                  spacing: __style.spacing10
                  visible: root.width >= 480 * __dp

                  // Nombre
                  Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: __style.row50
                    color: __style.polarColor
                    radius: __style.radius8
                    border.color: fieldNameInput.activeFocus ? __style.forestColor : __style.greyColor
                    border.width: fieldNameInput.activeFocus ? __style.width2 : __style.width1

                    TextField {
                      id: fieldNameInput
                      anchors.fill: parent
                      leftPadding: __style.margin12
                      rightPadding: __style.margin12
                      topPadding: 0; bottomPadding: 0
                      text: model.fieldName
                      color: __style.nightColor
                      font: __style.p5
                      placeholderText: qsTr("Ej: nombre")
                      placeholderTextColor: __style.darkGreyColor
                      background: Item {}
                      onTextEdited: model.fieldName = text
                    }
                  }

                  // Tipo
                  Rectangle {
                    Layout.preferredWidth: 100 * __dp
                    Layout.preferredHeight: __style.row50
                    color: __style.polarColor
                    radius: __style.radius8
                    border.color: __style.greyColor
                    border.width: __style.width1

                    ComboBox {
                      id: typeCombo
                      anchors.fill: parent
                      model: ["INT", "TEXT", "REAL", "DATE", "BOOLEAN", "BLOB"]
                      currentIndex: find(model.fieldType)
                      font: __style.p5
                      background: Item {}
                      contentItem: MMText {
                        text: typeCombo.displayText
                        font: __style.p5
                        color: __style.nightColor
                        leftPadding: __style.margin8
                        verticalAlignment: Text.AlignVCenter
                      }
                      onCurrentTextChanged: model.fieldType = currentText
                    }
                  }

                  // Tamaño (solo TEXT)
                  Rectangle {
                    Layout.preferredWidth: 90 * __dp
                    Layout.preferredHeight: __style.row50
                    color: typeCombo.currentText === "TEXT" ? __style.polarColor : __style.mediumGreenColor
                    radius: __style.radius8
                    border.color: sizeInput.activeFocus ? __style.forestColor : __style.greyColor
                    border.width: sizeInput.activeFocus ? __style.width2 : __style.width1

                    TextField {
                      id: sizeInput
                      anchors.fill: parent
                      leftPadding: __style.margin8
                      rightPadding: __style.margin8
                      topPadding: 0; bottomPadding: 0
                      text: model.fieldSize
                      enabled: typeCombo.currentText === "TEXT"
                      color: enabled ? __style.nightColor : __style.darkGreenColor
                      font: __style.p5
                      inputMethodHints: Qt.ImhDigitsOnly
                      placeholderText: qsTr("Ej: 255")
                      placeholderTextColor: __style.darkGreyColor
                      background: Item {}
                      onTextEdited: model.fieldSize = text
                    }
                  }

                  // Acciones
                  RowLayout {
                    Layout.preferredWidth: 100 * __dp
                    spacing: __style.spacing5

                    MMRoundButton { iconSource: __style.arrowUpIcon;   enabled: index > 0;                        onClicked: root.moveFieldUpRequested(index) }
                    MMRoundButton { iconSource: __style.arrowDownIcon; enabled: index < fieldsRepeater.count - 1; onClicked: root.moveFieldDownRequested(index) }
                    MMRoundButton { iconSource: __style.closeIcon;                                                onClicked: root.removeFieldRequested(index) }
                  }
                }

                // ── Móvil: apilado ──────────────────────────────────
                ColumnLayout {
                  anchors.fill: parent
                  anchors.margins: __style.margin8
                  spacing: __style.spacing5
                  visible: root.width < 480 * __dp

                  RowLayout {
                    Layout.fillWidth: true
                    spacing: __style.spacing10

                    // Nombre (móvil)
                    Rectangle {
                      Layout.fillWidth: true
                      Layout.preferredHeight: __style.row50
                      color: __style.polarColor
                      radius: __style.radius8
                      border.color: fieldNameMobile.activeFocus ? __style.forestColor : __style.greyColor
                      border.width: fieldNameMobile.activeFocus ? __style.width2 : __style.width1

                      TextField {
                        id: fieldNameMobile
                        anchors.fill: parent
                        leftPadding: __style.margin12
                        rightPadding: __style.margin12
                        topPadding: 0; bottomPadding: 0
                        text: model.fieldName
                        color: __style.nightColor
                        font: __style.p5
                        placeholderText: qsTr("Nombre campo")
                        placeholderTextColor: __style.darkGreyColor
                        background: Item {}
                        onTextEdited: model.fieldName = text
                      }
                    }

                    // Tipo (móvil)
                    Rectangle {
                      Layout.preferredWidth: 90 * __dp
                      Layout.preferredHeight: __style.row50
                      color: __style.polarColor
                      radius: __style.radius8
                      border.color: __style.greyColor
                      border.width: __style.width1

                      ComboBox {
                        id: typeComboMobile
                        anchors.fill: parent
                        model: ["INT", "TEXT", "REAL", "DATE", "BOOLEAN", "BLOB"]
                        currentIndex: find(model.fieldType)
                        font: __style.p5
                        background: Item {}
                        contentItem: MMText {
                          text: typeComboMobile.displayText
                          font: __style.p5
                          color: __style.nightColor
                          leftPadding: __style.margin8
                          verticalAlignment: Text.AlignVCenter
                        }
                        onCurrentTextChanged: model.fieldType = currentText
                      }
                    }
                  }

                  RowLayout {
                    Layout.fillWidth: true
                    spacing: __style.spacing10

                    // Tamaño (móvil)
                    Rectangle {
                      Layout.fillWidth: true
                      Layout.preferredHeight: __style.row50
                      color: typeComboMobile.currentText === "TEXT" ? __style.polarColor : __style.mediumGreenColor
                      radius: __style.radius8
                      border.color: sizeMobile.activeFocus ? __style.forestColor : __style.greyColor
                      border.width: sizeMobile.activeFocus ? __style.width2 : __style.width1

                      TextField {
                        id: sizeMobile
                        anchors.fill: parent
                        leftPadding: __style.margin8
                        rightPadding: __style.margin8
                        topPadding: 0; bottomPadding: 0
                        text: model.fieldSize
                        enabled: typeComboMobile.currentText === "TEXT"
                        color: enabled ? __style.nightColor : __style.darkGreenColor
                        font: __style.p5
                        inputMethodHints: Qt.ImhDigitsOnly
                        placeholderText: qsTr("Tamaño (solo TEXT)")
                        placeholderTextColor: __style.darkGreyColor
                        background: Item {}
                        onTextEdited: model.fieldSize = text
                      }
                    }

                    // Acciones (móvil)
                    MMRoundButton { iconSource: __style.arrowUpIcon;   enabled: index > 0;                        onClicked: root.moveFieldUpRequested(index) }
                    MMRoundButton { iconSource: __style.arrowDownIcon; enabled: index < fieldsRepeater.count - 1; onClicked: root.moveFieldDownRequested(index) }
                    MMRoundButton { iconSource: __style.closeIcon;                                                onClicked: root.removeFieldRequested(index) }
                  }
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
          onClicked: root.addFieldRequested()
        }
      }
    }

    // ── SECCIÓN 4: Mensaje de validación / resultado ──────────────────
    Rectangle {
      Layout.fillWidth: true
      implicitHeight: messageText.implicitHeight + __style.margin16 * 2
      visible: messageText.text !== ""
      color: root.messageType === "error" ? __style.negativeLightColor : __style.lightGreenColor
      radius: __style.radius12
      border.color: root.messageType === "error" ? __style.grapeColor : __style.grassColor
      border.width: __style.width1

      MMText {
        id: messageText
        anchors { left: parent.left; right: parent.right; top: parent.top; margins: __style.margin16 }
        font: __style.p5
        color: root.messageType === "error" ? __style.grapeColor : __style.forestColor
        wrapMode: Text.WordWrap
      }
    }
  }
}
