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
 * MMCreateTableForm — visual form to define table fields.
 * Presentation-only component; all logic lives in MMCreateTableDrawer.
 * Fully responsive: single layout that adapts from narrow phone to wide desktop.
 *
 * Breakpoint: compact = width < 400 dp
 *   compact  → tipo y tamaño en segunda fila; acciones iconos pequeños
 *   wide     → todo en una sola fila
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

  // Breakpoint central — todo el form lo lee desde aquí
  readonly property bool compact: width < 400 * __dp

  // Anchos de columna responsivos — disponibles para cabecera y filas
  readonly property real typeColWidth:    Math.max(70 * __dp, root.width * 0.22)
  readonly property real sizeColWidth:    Math.max(55 * __dp, root.width * 0.17)
  readonly property real actionsColWidth: 3 * __style.row40 + 2 * __style.spacing5

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

    // ── SECCIÓN 1: Info de la BD ────────────────────────────────────────
    Rectangle {
      Layout.fillWidth: true
      implicitHeight: dbInfoColumn.implicitHeight + __style.margin16 * 2
      color: __style.lightGreenColor
      radius: __style.radius12
      border.color: __style.greyColor
      border.width: __style.width1

      ColumnLayout {
        id: dbInfoColumn
        anchors { top: parent.top; left: parent.left; right: parent.right; margins: __style.margin16 }
        spacing: __style.spacing10

        MMText {
          text: qsTr("Información de Base de Datos")
          font: __style.t3
          color: __style.forestColor
          wrapMode: Text.WordWrap
          Layout.fillWidth: true
        }

        // Nombre BD
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
              topPadding: 0; bottomPadding: 0
              readOnly: true
              color: __style.nightColor
              font: __style.p5
              placeholderText: qsTr("Ej: miproyecto")
              placeholderTextColor: __style.darkGreyColor
              background: Item {}
            }
          }
        }

        // Ubicación BD
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
              topPadding: 0; bottomPadding: 0
              readOnly: true
              color: __style.nightColor
              font: __style.p5
              placeholderText: qsTr("Ruta de almacenamiento")
              placeholderTextColor: __style.darkGreyColor
              background: Item {}
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

    // ── SECCIÓN 2: Nombre de la tabla ───────────────────────────────────
    Rectangle {
      Layout.fillWidth: true
      implicitHeight: tableNameColumn.implicitHeight + __style.margin16 * 2
      color: __style.lightGreenColor
      radius: __style.radius12
      border.color: __style.greyColor
      border.width: __style.width1

      ColumnLayout {
        id: tableNameColumn
        anchors { top: parent.top; left: parent.left; right: parent.right; margins: __style.margin16 }
        spacing: __style.spacing10

        MMText {
          text: qsTr("Nombre de la Tabla")
          font: __style.t3
          color: __style.forestColor
          wrapMode: Text.WordWrap
          Layout.fillWidth: true
        }

        MMText {
          text: qsTr("Solo letras, números y guiones bajos")
          font: __style.p6
          color: __style.nightColor
          wrapMode: Text.WordWrap
          Layout.fillWidth: true
        }

        MMTextInput {
          id: tableNameField
          Layout.fillWidth: true
          placeholderText: qsTr("Ej: usuarios, productos, clientes")
        }
      }
    }

    // ── SECCIÓN 3: Definición de campos ────────────────────────────────
    Rectangle {
      Layout.fillWidth: true
      Layout.fillHeight: true
      color: __style.lightGreenColor
      radius: __style.radius12
      border.color: __style.greyColor
      border.width: __style.width1

      ColumnLayout {
        anchors.fill: parent
        anchors.margins: __style.margin12
        spacing: __style.spacing10

        MMText {
          text: qsTr("Definición de Campos")
          font: __style.t3
          color: __style.forestColor
          wrapMode: Text.WordWrap
          Layout.fillWidth: true
        }

        // Cabecera de columnas — solo en modo ancho
        Rectangle {
          Layout.fillWidth: true
          implicitHeight: __style.row40
          color: __style.forestColor
          radius: __style.radius8
          visible: !root.compact

          RowLayout {
            anchors.fill: parent
            anchors.leftMargin: __style.margin12
            anchors.rightMargin: __style.margin12
            spacing: __style.spacing10

            MMText { text: qsTr("Nombre");   font: __style.t4; color: __style.polarColor; Layout.fillWidth: true }
            MMText { text: qsTr("Tipo");     font: __style.t4; color: __style.polarColor; Layout.preferredWidth: typeColWidth }
            MMText { text: qsTr("Tamaño");   font: __style.t4; color: __style.polarColor; Layout.preferredWidth: sizeColWidth }
            MMText { text: qsTr("Acciones"); font: __style.t4; color: __style.polarColor; Layout.preferredWidth: actionsColWidth }
          }
        }

        // Lista de campos con scroll
        ScrollView {
          Layout.fillWidth: true
          Layout.fillHeight: true
          clip: true
          contentWidth: availableWidth   // evita scroll horizontal

          ColumnLayout {
            width: parent.width
            spacing: __style.spacing5

            Repeater {
              id: fieldsRepeater

              delegate: Rectangle {
                // Altura: en compacto apila dos filas
                readonly property real _rowH: __style.row50
                readonly property real _gap:  __style.spacing5
                readonly property real _pad:  __style.margin8
                implicitWidth:  parent?.width ?? 0
                implicitHeight: root.compact
                                ? (_pad * 2 + _rowH * 2 + _gap)
                                : (_pad * 2 + _rowH)
                color: index % 2 === 0 ? __style.polarColor : __style.lightGreenColor
                radius: __style.radius8
                border.color: __style.greyColor
                border.width: __style.width1

                // Acceso a anchos de columna desde root
                readonly property real _typeW:    root.typeColWidth
                readonly property real _sizeW:    root.sizeColWidth
                readonly property real _actionsW: root.actionsColWidth

                // ── MODO ANCHO: una fila ──────────────────────────────
                RowLayout {
                  anchors { left: parent.left; right: parent.right; top: parent.top; margins: parent._pad }
                  height: parent._rowH
                  spacing: __style.spacing10
                  visible: !root.compact

                  // Nombre
                  Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height
                    color: __style.polarColor
                    radius: __style.radius8
                    border.color: fieldNameWide.activeFocus ? __style.forestColor : __style.greyColor
                    border.width: fieldNameWide.activeFocus ? __style.width2 : __style.width1

                    TextField {
                      id: fieldNameWide
                      anchors.fill: parent
                      leftPadding: __style.margin12; rightPadding: __style.margin12
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
                    Layout.preferredWidth: parent.parent._typeW
                    Layout.preferredHeight: parent.height
                    color: __style.polarColor
                    radius: __style.radius8
                    border.color: __style.greyColor
                    border.width: __style.width1

                    ComboBox {
                      id: typeComboWide
                      anchors.fill: parent
                      model: ["INT", "TEXT", "REAL", "DATE", "BOOLEAN", "BLOB"]
                      currentIndex: find(modelData !== undefined ? modelData : (fieldsRepeater.model.get ? fieldsRepeater.model.get(index).fieldType : "TEXT"))
                      font: __style.p5
                      background: Item {}
                      contentItem: MMText {
                        text: typeComboWide.displayText
                        font: __style.p5
                        color: __style.nightColor
                        leftPadding: __style.margin8
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                      }
                      onCurrentTextChanged: model.fieldType = currentText
                    }
                  }

                  // Tamaño
                  Rectangle {
                    Layout.preferredWidth: parent.parent._sizeW
                    Layout.preferredHeight: parent.height
                    color: typeComboWide.currentText === "TEXT" ? __style.polarColor : __style.mediumGreenColor
                    radius: __style.radius8
                    border.color: sizeWide.activeFocus ? __style.forestColor : __style.greyColor
                    border.width: sizeWide.activeFocus ? __style.width2 : __style.width1

                    TextField {
                      id: sizeWide
                      anchors.fill: parent
                      leftPadding: __style.margin8; rightPadding: __style.margin8
                      topPadding: 0; bottomPadding: 0
                      text: model.fieldSize
                      enabled: typeComboWide.currentText === "TEXT"
                      color: enabled ? __style.nightColor : __style.darkGreenColor
                      font: __style.p5
                      inputMethodHints: Qt.ImhDigitsOnly
                      placeholderText: qsTr("255")
                      placeholderTextColor: __style.darkGreyColor
                      background: Item {}
                      onTextEdited: model.fieldSize = text
                    }
                  }

                  // Acciones
                  RowLayout {
                    Layout.preferredWidth: parent.parent._actionsW
                    spacing: __style.spacing5
                    MMRoundButton { iconSource: __style.arrowUpIcon;   enabled: index > 0;                        onClicked: root.moveFieldUpRequested(index) }
                    MMRoundButton { iconSource: __style.arrowDownIcon; enabled: index < fieldsRepeater.count - 1; onClicked: root.moveFieldDownRequested(index) }
                    MMRoundButton { iconSource: __style.closeIcon;                                                onClicked: root.removeFieldRequested(index) }
                  }
                }

                // ── MODO COMPACTO: dos filas ──────────────────────────
                ColumnLayout {
                  anchors { left: parent.left; right: parent.right; top: parent.top; margins: parent._pad }
                  spacing: parent._gap
                  visible: root.compact

                  // Fila 1: nombre + botón eliminar
                  RowLayout {
                    Layout.fillWidth: true
                    spacing: __style.spacing10

                    Rectangle {
                      Layout.fillWidth: true
                      Layout.preferredHeight: __style.row50
                      color: __style.polarColor
                      radius: __style.radius8
                      border.color: fieldNameCompact.activeFocus ? __style.forestColor : __style.greyColor
                      border.width: fieldNameCompact.activeFocus ? __style.width2 : __style.width1

                      TextField {
                        id: fieldNameCompact
                        anchors.fill: parent
                        leftPadding: __style.margin12; rightPadding: __style.margin12
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

                    MMRoundButton {
                      iconSource: __style.closeIcon
                      onClicked: root.removeFieldRequested(index)
                    }
                  }

                  // Fila 2: tipo + tamaño + mover arriba/abajo
                  RowLayout {
                    Layout.fillWidth: true
                    spacing: __style.spacing10

                    // Tipo
                    Rectangle {
                      Layout.fillWidth: true
                      Layout.preferredHeight: __style.row50
                      color: __style.polarColor
                      radius: __style.radius8
                      border.color: __style.greyColor
                      border.width: __style.width1

                      ComboBox {
                        id: typeComboCompact
                        anchors.fill: parent
                        model: ["INT", "TEXT", "REAL", "DATE", "BOOLEAN", "BLOB"]
                        currentIndex: find(modelData !== undefined ? modelData : (fieldsRepeater.model.get ? fieldsRepeater.model.get(index).fieldType : "TEXT"))
                        font: __style.p5
                        background: Item {}
                        contentItem: MMText {
                          text: typeComboCompact.displayText
                          font: __style.p5
                          color: __style.nightColor
                          leftPadding: __style.margin8
                          verticalAlignment: Text.AlignVCenter
                          elide: Text.ElideRight
                        }
                        onCurrentTextChanged: model.fieldType = currentText
                      }
                    }

                    // Tamaño
                    Rectangle {
                      Layout.preferredWidth: Math.max(50 * __dp, root.width * 0.22)
                      Layout.preferredHeight: __style.row50
                      color: typeComboCompact.currentText === "TEXT" ? __style.polarColor : __style.mediumGreenColor
                      radius: __style.radius8
                      border.color: sizeCompact.activeFocus ? __style.forestColor : __style.greyColor
                      border.width: sizeCompact.activeFocus ? __style.width2 : __style.width1

                      TextField {
                        id: sizeCompact
                        anchors.fill: parent
                        leftPadding: __style.margin8; rightPadding: __style.margin8
                        topPadding: 0; bottomPadding: 0
                        text: model.fieldSize
                        enabled: typeComboCompact.currentText === "TEXT"
                        color: enabled ? __style.nightColor : __style.darkGreenColor
                        font: __style.p5
                        inputMethodHints: Qt.ImhDigitsOnly
                        placeholderText: qsTr("255")
                        placeholderTextColor: __style.darkGreyColor
                        background: Item {}
                        onTextEdited: model.fieldSize = text
                      }
                    }

                    // Mover
                    MMRoundButton { iconSource: __style.arrowUpIcon;   enabled: index > 0;                        onClicked: root.moveFieldUpRequested(index) }
                    MMRoundButton { iconSource: __style.arrowDownIcon; enabled: index < fieldsRepeater.count - 1; onClicked: root.moveFieldDownRequested(index) }
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

    // ── SECCIÓN 4: Mensaje de validación ────────────────────────────────
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
