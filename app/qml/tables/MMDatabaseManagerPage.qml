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
 * MMDatabaseManagerPage — database table manager.
 * Follows the MMPage pattern: view only, no business logic.
 * All communication with the backend is done via signals and properties.
 */

MMPage {
  id: root

  // ── Propiedades de entrada (el Controller las asigna) ─────────────────
  property var    tableModel:   null
  property var    tableList:    []
  property string currentTable: ""
  property int    rowCount:     0
  property string errorMessage: ""

  property var selectedRowIndex: -1

  // ── Señales de salida (el Controller las escucha) ─────────────────────
  signal tableSelected(string name)
  signal addRowRequested()
  signal removeRowRequested(int rowIndex)
  signal submitChangesRequested()
  signal revertChangesRequested()
  signal filterRequested(string text)
  signal clearFilterRequested()
  signal createTableRequested()
  signal createDatabaseRequested()
  signal backClicked()

  // ── Cabecera ──────────────────────────────────────────────────────────
  pageHeader {
    title: qsTr("Gestor de Base de Datos")
    titleFont: __style.h3
    baseHeaderHeight: __style.row60
    backVisible: true
  }

  onBackClicked: root.backClicked()

  // ── Contenido principal ───────────────────────────────────────────────
  pageContent: ColumnLayout {
    width: parent.width
    height: parent.height
    spacing: __style.spacing12

    // ── Notificación de error ─────────────────────────────────────────
    MMNotificationBox {
      Layout.fillWidth: true
      visible: root.errorMessage !== ""
      type: MMNotificationBox.Types.Error
      title: qsTr("Error")
      description: root.errorMessage
    }

    // ── Barra de selector de tabla + indicador de estado ─────────────
    Rectangle {
      Layout.fillWidth: true
      implicitHeight: __style.row60
      color: __style.forestColor
      radius: __style.radius12

      RowLayout {
        anchors.fill: parent
        anchors.leftMargin: __style.margin16
        anchors.rightMargin: __style.margin16
        spacing: __style.spacing12

        MMText {
          text: qsTr("Tabla:")
          font: __style.t3
          color: __style.polarColor
        }

        // ComboBox estilizado con tokens
        Rectangle {
          Layout.preferredWidth: 220 * __dp
          Layout.preferredHeight: __style.row50
          color: __style.polarColor
          radius: __style.radius12

          ComboBox {
            id: tableComboBox
            anchors.fill: parent
            anchors.margins: 2 * __dp
            model: root.tableList
            font: __style.p5

            background: Rectangle { color: __style.transparentColor }

            contentItem: MMText {
              text: tableComboBox.displayText
              font: __style.p5
              color: __style.nightColor
              leftPadding: __style.margin12
              verticalAlignment: Text.AlignVCenter
            }

            onCurrentTextChanged: {
              if (currentText !== "") {
                root.tableSelected(currentText)
              }
            }
          }
        }

        Item { Layout.fillWidth: true }

        // Botón crear BD
        MMButton {
          text: qsTr("Nueva BD")
          type: MMButton.Types.Secondary
          size: MMButton.Sizes.Small
          onClicked: root.createDatabaseRequested()
        }

        // Botón crear tabla
        MMButton {
          text: qsTr("Nueva Tabla")
          size: MMButton.Sizes.Small
          onClicked: root.createTableRequested()
        }
      }
    }

    // ── Área principal (tabla + panel lateral) ────────────────────────
    RowLayout {
      Layout.fillWidth: true
      Layout.fillHeight: true
      spacing: __style.spacing12

      // ── Tabla de datos ─────────────────────────────────────────────
      Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: true
        color: __style.polarColor
        radius: __style.radius12
        border.color: __style.greyColor
        border.width: __style.width1

        ColumnLayout {
          anchors.fill: parent
          anchors.margins: __style.margin12
          spacing: __style.spacing12

          // Sub-cabecera de tabla
          Rectangle {
            Layout.fillWidth: true
            implicitHeight: __style.row40
            color: __style.lightGreenColor
            radius: __style.radius8

            RowLayout {
              anchors.fill: parent
              anchors.leftMargin: __style.margin12
              anchors.rightMargin: __style.margin12
              spacing: __style.spacing12

              MMText {
                text: root.currentTable || qsTr("Selecciona una tabla")
                font: __style.t3
                color: __style.forestColor
              }

              Item { Layout.fillWidth: true }

              MMText {
                text: qsTr("Registros: ") + root.rowCount
                font: __style.p6
                color: __style.nightColor
              }
            }
          }

          // Vista de tabla
          TableView {
            id: dataTableView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: root.tableModel
            alternatingRowColors: true
            clip: true

            delegate: Rectangle {
              implicitWidth: 120 * __dp
              implicitHeight: __style.row40
              color: row === root.selectedRowIndex
                     ? __style.grassColor
                     : (index % 2 === 0 ? __style.polarColor : __style.lightGreenColor)

              border.color: column === 0 ? __style.greyColor : __style.transparentColor
              border.width: __style.width1

              TextInput {
                anchors.fill: parent
                anchors.margins: __style.margin4
                text: display ?? ""
                color: row === root.selectedRowIndex ? __style.forestColor : __style.nightColor
                font: __style.p5
                selectByMouse: true
                readOnly: false
                verticalAlignment: TextInput.AlignVCenter

                onEditingFinished: {
                  if (root.tableModel) {
                    root.tableModel.setData(
                      root.tableModel.index(row, column),
                      text
                    )
                  }
                }

                MouseArea {
                  anchors.fill: parent
                  onPressed: function(mouse) {
                    root.selectedRowIndex = row
                    mouse.accepted = false
                  }
                }
              }
            }
          }

          // Barra de búsqueda / filtro
          RowLayout {
            Layout.fillWidth: true
            spacing: __style.spacing10

            MMSearchInput {
              id: searchField
              Layout.fillWidth: true
              placeholderText: qsTr("Filtrar datos...")
            }

            MMButton {
              text: qsTr("Buscar")
              size: MMButton.Sizes.Small
              onClicked: {
                if (searchField.text) {
                  root.filterRequested(searchField.text)
                } else {
                  root.clearFilterRequested()
                }
              }
            }

            MMButton {
              text: qsTr("Limpiar")
              type: MMButton.Types.Secondary
              size: MMButton.Sizes.Small
              onClicked: {
                searchField.text = ""
                root.clearFilterRequested()
              }
            }
          }
        }
      }

      // ── Panel lateral de controles ─────────────────────────────────
      Rectangle {
        Layout.preferredWidth: 180 * __dp
        Layout.fillHeight: true
        color: __style.lightGreenColor
        radius: __style.radius12
        border.color: __style.greyColor
        border.width: __style.width1

        ColumnLayout {
          anchors.fill: parent
          anchors.margins: __style.margin12
          spacing: __style.spacing12

          MMText {
            text: qsTr("CONTROLES")
            font: __style.t3
            color: __style.forestColor
            Layout.alignment: Qt.AlignHCenter
          }

          Rectangle {
            Layout.fillWidth: true
            height: __style.width1
            color: __style.greyColor
          }

          // Agregar registro
          MMButton {
            Layout.fillWidth: true
            text: qsTr("Agregar")
            iconSourceLeft: __style.addIcon
            onClicked: root.addRowRequested()
          }

          // Eliminar registro
          MMButton {
            Layout.fillWidth: true
            text: qsTr("Eliminar")
            type: MMButton.Types.Secondary
            enabled: root.selectedRowIndex >= 0
            onClicked: {
              root.removeRowRequested(root.selectedRowIndex)
              root.selectedRowIndex = -1
            }
          }

          // Guardar cambios
          MMButton {
            Layout.fillWidth: true
            text: qsTr("Guardar")
            onClicked: root.submitChangesRequested()
          }

          // Revertir cambios
          MMButton {
            Layout.fillWidth: true
            text: qsTr("Revertir")
            type: MMButton.Types.Secondary
            onClicked: root.revertChangesRequested()
          }

          Rectangle {
            Layout.fillWidth: true
            height: __style.width1
            color: __style.greyColor
          }

          // Panel de información
          Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: __style.polarColor
            radius: __style.radius8
            border.color: __style.greyColor
            border.width: __style.width1

            ColumnLayout {
              anchors.fill: parent
              anchors.margins: __style.margin8
              spacing: __style.spacing10

              MMText {
                text: qsTr("INFO")
                font: __style.t4
                color: __style.forestColor
              }

              Rectangle {
                Layout.fillWidth: true
                height: __style.width1
                color: __style.greyColor
              }

              MMText {
                text: qsTr("Tabla:\n") + (root.currentTable || "-")
                font: __style.p6
                color: __style.nightColor
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
              }

              MMText {
                text: qsTr("Registros:\n") + root.rowCount
                font: __style.p6
                color: __style.nightColor
                Layout.fillWidth: true
              }

              Item { Layout.fillHeight: true }
            }
          }
        }
      }
    }
  }

  Component.onCompleted: {
    console.log("MMDatabaseManagerPage inicializado")
  }
}
