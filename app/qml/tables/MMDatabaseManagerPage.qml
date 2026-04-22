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
 * MMDatabaseManagerPage — database table manager, vertical scrollable layout.
 *
 * Order top → bottom:
 *   1. Error notification (when present)
 *   2. Table selector + Nueva BD / Nueva Tabla buttons
 *   3. Data table (expands with rows, page scrolls)
 *   4. Search / filter bar
 *   5. Action buttons in 2-column grid
 */

MMPage {
  id: root

  // ── Propiedades de entrada ────────────────────────────────────────────
  property var    tableModel:   null
  property var    tableList:    []
  property string currentTable: ""
  property int    rowCount:     0
  property string errorMessage: ""
  property var    selectedRowIndex: -1

  // ── Señales de salida ─────────────────────────────────────────────────
  signal tableSelected(string name)
  signal addRowRequested()
  signal removeRowRequested(int rowIndex)
  signal submitChangesRequested()
  signal revertChangesRequested()
  signal filterRequested(string text)
  signal clearFilterRequested()
  signal createTableRequested()
  signal createDatabaseRequested()

  // ── Cabecera ──────────────────────────────────────────────────────────
  pageHeader {
    title: qsTr("Gestor de Base de Datos")
    //titleFont: __style.h3
    baseHeaderHeight: __style.row60
    backVisible: true
  }

  // ── Contenido: scroll vertical completo ──────────────────────────────
  pageContent: MMScrollView {
    width: parent.width
    height: parent.height

    ColumnLayout {
      width: parent.width
      spacing: __style.spacing12

      // ── 1. Error ────────────────────────────────────────────────────
      MMNotificationBox {
        Layout.fillWidth: true
        visible: root.errorMessage !== ""
        type: MMNotificationBox.Types.Error
        title: qsTr("Error")
        description: root.errorMessage
      }

      // ── 2. Selector de tabla ────────────────────────────────────────
      Rectangle {
        Layout.fillWidth: true
        implicitHeight: selectorCol.implicitHeight + __style.margin16 * 2
        color: __style.forestColor
        radius: __style.radius12

        ColumnLayout {
          id: selectorCol
          anchors { left: parent.left; right: parent.right; top: parent.top; margins: __style.margin16 }
          spacing: __style.spacing10

          MMText {
            text: qsTr("Tabla activa")
            font: __style.t3
            color: __style.polarColor
          }

          // ComboBox ancho completo
          Rectangle {
            Layout.fillWidth: true
            implicitHeight: __style.row50
            color: __style.polarColor
            radius: __style.radius12

            ComboBox {
              id: tableCombo
              anchors.fill: parent
              anchors.margins: 2 * __dp
              model: root.tableList
              font: __style.p5
              background: Rectangle { color: __style.transparentColor }
              contentItem: MMText {
                text: tableCombo.displayText
                font: __style.p5
                color: __style.nightColor
                leftPadding: __style.margin12
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
              }
              onCurrentTextChanged: if (currentText !== "") root.tableSelected(currentText)
            }
          }

          // Botones Nueva BD / Nueva Tabla
          RowLayout {
            Layout.fillWidth: true
            spacing: __style.spacing10

            MMButton {
              text: qsTr("Nueva BD")
              size: MMButton.Sizes.Small
              Layout.fillWidth: true
              onClicked: root.createDatabaseRequested()
            }

            MMButton {
              text: qsTr("Nueva Tabla")
              size: MMButton.Sizes.Small
              Layout.fillWidth: true
              onClicked: {
                console.log("msj: Nueva tabla");
                root.createTableRequested()
              }
            }
          }
        }
      }

      // ── 3. Tabla de datos ───────────────────────────────────────────
      Rectangle {
        Layout.fillWidth: true
        // Altura: encabezado + filas; mínimo 120dp para que siempre sea visible
        implicitHeight: Math.max(
          120 * __dp,
          tableHeader.height + (root.rowCount * __style.row40) + __style.margin12 * 2
        )
        color: __style.polarColor
        radius: __style.radius12
        border.color: __style.greyColor
        border.width: __style.width1

        ColumnLayout {
          anchors.fill: parent
          anchors.margins: __style.margin12
          spacing: __style.spacing10

          // Sub-cabecera: nombre de tabla + conteo
          Rectangle {
            id: tableHeader
            Layout.fillWidth: true
            implicitHeight: __style.row40
            color: __style.lightGreenColor
            radius: __style.radius8

            RowLayout {
              anchors.fill: parent
              anchors.leftMargin: __style.margin12
              anchors.rightMargin: __style.margin12
              spacing: __style.spacing10

              MMText {
                text: root.currentTable || qsTr("Selecciona una tabla")
                font: __style.t3
                color: __style.forestColor
                elide: Text.ElideRight
                Layout.fillWidth: true
              }

              MMText {
                text: root.rowCount + qsTr(" reg.")
                font: __style.p6
                color: __style.nightColor
              }

              // Fila seleccionada
              MMText {
                visible: root.selectedRowIndex >= 0
                text: qsTr("Fila: ") + (root.selectedRowIndex + 1)
                font: __style.p6
                color: __style.grapeColor
              }
            }
          }

          // TableView sin altura fija — se expande con los datos
          TableView {
            id: dataTableView
            Layout.fillWidth: true
            implicitHeight: root.rowCount * __style.row40
            model: root.tableModel
            clip: true

            delegate: Rectangle {
              implicitWidth: Math.max(80 * __dp, dataTableView.width / Math.max(1, dataTableView.columns))
              implicitHeight: __style.row40
              color: row === root.selectedRowIndex
                     ? __style.grassColor
                     : (row % 2 === 0 ? __style.polarColor : __style.lightGreenColor)
              border.color: __style.greyColor
              border.width: __style.width1

              TextInput {
                anchors.fill: parent
                anchors.margins: __style.margin4
                text: display ?? ""
                color: row === root.selectedRowIndex ? __style.forestColor : __style.nightColor
                font: __style.p5
                selectByMouse: true
                verticalAlignment: TextInput.AlignVCenter
                clip: true

                onEditingFinished: {
                  if (root.tableModel)
                    root.tableModel.setData(root.tableModel.index(row, column), text)
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
        }
      }

      // ── 4. Barra de búsqueda ────────────────────────────────────────
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
          onClicked: searchField.text ? root.filterRequested(searchField.text) : root.clearFilterRequested()
        }

        MMButton {
          text: qsTr("Limpiar")
          type: MMButton.Types.Secondary
          size: MMButton.Sizes.Small
          onClicked: { searchField.text = ""; root.clearFilterRequested() }
        }
      }

      // ── 5. Botones de acción en grid 2 columnas ─────────────────────
      Rectangle {
        Layout.fillWidth: true
        implicitHeight: actionsGrid.implicitHeight + __style.margin16 * 2
        color: __style.lightGreenColor
        radius: __style.radius12
        border.color: __style.greyColor
        border.width: __style.width1

        GridLayout {
          id: actionsGrid
          anchors { left: parent.left; right: parent.right; top: parent.top; margins: __style.margin16 }
          columns: 2
          columnSpacing: __style.spacing10
          rowSpacing: __style.spacing10

          MMButton {
            Layout.fillWidth: true
            text: qsTr("Agregar fila")
            iconSourceLeft: __style.addIcon
            onClicked: root.addRowRequested()
          }

          MMButton {
            Layout.fillWidth: true
            text: qsTr("Eliminar fila")
            type: MMButton.Types.Secondary
            enabled: root.selectedRowIndex >= 0
            onClicked: { root.removeRowRequested(root.selectedRowIndex); root.selectedRowIndex = -1 }
          }

          MMButton {
            Layout.fillWidth: true
            text: qsTr("Guardar cambios")
            onClicked: root.submitChangesRequested()
          }

          MMButton {
            Layout.fillWidth: true
            text: qsTr("Revertir cambios")
            type: MMButton.Types.Secondary
            onClicked: root.revertChangesRequested()
          }
        }
      }

      // Espacio inferior para que el último botón no quede pegado al borde
      Item { implicitHeight: __style.spacing20 }
    }
  }

  Component.onCompleted: console.log("MMDatabaseManagerPage inicializado")
}
