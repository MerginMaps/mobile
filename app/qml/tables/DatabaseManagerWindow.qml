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
 * DatabaseManagerWindow — gestor de tablas de base de datos.
 * Embebido como MMPage dentro del StackView principal de la app.
 * Sustituye el antiguo ApplicationWindow independiente.
 */

MMPage {
  id: root

  // ── Estados de vista ──────────────────────────────────────────────────
  enum ViewState { Idle, Loading, Error, DataLoaded }
  property int viewState: DatabaseManagerWindow.ViewState.Idle

  property var selectedRowIndex: -1

  // ── Cabecera ──────────────────────────────────────────────────────────
  pageHeader {
    title: qsTr("Gestor de Base de Datos")
    titleFont: __style.h3
    baseHeaderHeight: __style.row60
  }

  // ── Contenido principal ───────────────────────────────────────────────
  pageContent: ColumnLayout {
    width: parent.width
    height: parent.height
    spacing: __style.spacing12

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
            model: dbManager.tableList
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
                dbManager.setCurrentTable(currentText)
              }
            }
          }
        }

        Item { Layout.fillWidth: true }

        // Indicador de estado
        Rectangle {
          Layout.preferredWidth: 160 * __dp
          Layout.preferredHeight: __style.row40
          radius: __style.radius12
          visible: root.viewState !== DatabaseManagerWindow.ViewState.Idle
          color: {
            switch (root.viewState) {
              case DatabaseManagerWindow.ViewState.Error:      return __style.negativeColor
              case DatabaseManagerWindow.ViewState.Loading:    return __style.warningColor
              case DatabaseManagerWindow.ViewState.DataLoaded: return __style.grassColor
              default: return __style.transparentColor
            }
          }

          MMText {
            anchors.centerIn: parent
            font: __style.t4
            color: __style.forestColor
            text: {
              switch (root.viewState) {
                case DatabaseManagerWindow.ViewState.Error:      return qsTr("Error")
                case DatabaseManagerWindow.ViewState.Loading:    return qsTr("Cargando...")
                case DatabaseManagerWindow.ViewState.DataLoaded: return qsTr("Datos cargados")
                default: return ""
              }
            }
          }
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
                text: dbManager.currentTable || qsTr("Selecciona una tabla")
                font: __style.t3
                color: __style.forestColor
              }

              Item { Layout.fillWidth: true }

              MMText {
                text: qsTr("Registros: ") + dbManager.getRowCount()
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
            model: dbManager.tableModel
            alternatingRowColors: true
            clip: true

            delegate: Rectangle {
              implicitWidth: 120 * __dp
              implicitHeight: __style.row40
              color: row === selectedRowIndex
                     ? __style.grassColor
                     : (index % 2 === 0 ? __style.polarColor : __style.lightGreenColor)

              border.color: column === 0 ? __style.greyColor : __style.transparentColor
              border.width: __style.width1

              TextInput {
                anchors.fill: parent
                anchors.margins: __style.margin4
                text: display ?? ""
                color: row === selectedRowIndex ? __style.forestColor : __style.nightColor
                font: __style.p5
                selectByMouse: true
                readOnly: false
                verticalAlignment: TextInput.AlignVCenter

                onEditingFinished: {
                  if (dbManager.tableModel) {
                    dbManager.tableModel.setData(
                      dbManager.tableModel.index(row, column),
                      text
                    )
                  }
                }

                MouseArea {
                  anchors.fill: parent
                  onPressed: function(mouse) {
                    selectedRowIndex = row
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
                  dbManager.filterTable(searchField.text)
                } else {
                  dbManager.clearFilter()
                }
              }
            }

            MMButton {
              text: qsTr("Limpiar")
              type: MMButton.Types.Secondary
              size: MMButton.Sizes.Small
              onClicked: {
                searchField.text = ""
                dbManager.clearFilter()
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
            onClicked: {
              if (!dbManager.addRow()) {
                errorDrawer.errorMsg = qsTr("No se pudo agregar registro")
                errorDrawer.open()
              }
            }
          }

          // Eliminar registro
          MMButton {
            Layout.fillWidth: true
            text: qsTr("Eliminar")
            type: MMButton.Types.Secondary
            enabled: selectedRowIndex >= 0
            onClicked: {
              if (!dbManager.removeRow(selectedRowIndex)) {
                errorDrawer.errorMsg = qsTr("No se pudo eliminar registro")
                errorDrawer.open()
              } else {
                selectedRowIndex = -1
              }
            }
          }

          // Guardar cambios
          MMButton {
            Layout.fillWidth: true
            text: qsTr("Guardar")
            onClicked: {
              if (dbManager.submitChanges()) {
                root.viewState = DatabaseManagerWindow.ViewState.DataLoaded
              } else {
                root.viewState = DatabaseManagerWindow.ViewState.Error
              }
            }
          }

          // Revertir cambios
          MMButton {
            Layout.fillWidth: true
            text: qsTr("Revertir")
            type: MMButton.Types.Secondary
            onClicked: dbManager.revertChanges()
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
                text: qsTr("Tabla:\n") + (dbManager.currentTable || "-")
                font: __style.p6
                color: __style.nightColor
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
              }

              MMText {
                text: qsTr("Registros:\n") + dbManager.getRowCount()
                font: __style.p6
                color: __style.nightColor
                Layout.fillWidth: true
              }

              Item { Layout.fillHeight: true }

              // Caja de error inline
              MMNotificationBox {
                Layout.fillWidth: true
                visible: dbManager.lastError !== ""
                type: MMNotificationBox.Types.Error
                title: qsTr("Error")
                description: dbManager.lastError
              }
            }
          }
        }
      }
    }
  }

  // ── Drawer de error ───────────────────────────────────────────────────
  MMDrawer {
    id: errorDrawer

    property string errorMsg: ""

    drawerHeader.title: qsTr("Error")

    drawerContent: ColumnLayout {
      width: parent.width
      spacing: __style.spacing12

      MMText {
        text: errorDrawer.errorMsg
        font: __style.p5
        color: __style.nightColor
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
      }

      MMButton {
        text: qsTr("Aceptar")
        Layout.fillWidth: true
        onClicked: errorDrawer.close()
      }
    }
  }

  // ── Conexiones con el backend ──────────────────────────────────────────
  Connections {
    target: dbManager

    function onErrorOccurred(errorMessage) {
      root.viewState = DatabaseManagerWindow.ViewState.Error
    }

    function onTableModelChanged() {
      root.viewState = DatabaseManagerWindow.ViewState.DataLoaded
      selectedRowIndex = -1
    }

    function onDataChanged() {
      root.viewState = DatabaseManagerWindow.ViewState.DataLoaded
    }
  }

  Component.onCompleted: {
    console.log("DatabaseManagerWindow inicializado")
  }
}
