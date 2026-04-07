import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

ApplicationWindow {
    id: root
    visible: true
    width: 1200
    height: 700
    title: "Sistema de Gestión de Datos"

    // Conexión con el backend C++
    // Asume que DynamicDatabaseManager está registrado en QML como 'dbManager'

    color: "#f5f5f5"

    // Estados de la aplicación
    enum ViewState {
        Idle,
        Loading,
        Error,
        DataLoaded
    }

    property int viewState: DatabaseManagerWindow.ViewState.Idle
    property var selectedRowIndex: -1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15

        // ============ BARRA DE HERRAMIENTAS ============
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: "#2c3e50"
            radius: 5

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                Text {
                    text: "📊 Base de Datos"
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }

                Rectangle {
                    Layout.preferredWidth: 1
                    Layout.fillHeight: true
                    color: "#34495e"
                }

                ComboBox {
                    id: tableComboBox
                    Layout.preferredWidth: 250
                    model: dbManager.tableList

                    onCurrentTextChanged: {
                        if (currentText !== "") {
                            dbManager.setCurrentTable(currentText)
                        }
                    }

                    background: Rectangle {
                        color: "#34495e"
                        border.color: "#7f8c8d"
                        border.width: 1
                        radius: 3
                    }

                    contentItem: Text {
                        text: tableComboBox.displayText
                        color: "white"
                        leftPadding: 8
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Item { Layout.fillWidth: true }

                // Indicador de estado
                Rectangle {
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 35
                    color: viewState === DatabaseManagerWindow.ViewState.Error ? "#e74c3c" :
                           viewState === DatabaseManagerWindow.ViewState.Loading ? "#f39c12" :
                           viewState === DatabaseManagerWindow.ViewState.DataLoaded ? "#27ae60" : "transparent"
                    radius: 3
                    visible: viewState !== DatabaseManagerWindow.ViewState.Idle

                    Text {
                        anchors.centerIn: parent
                        color: "white"
                        text: viewState === DatabaseManagerWindow.ViewState.Error ? "❌ Error" :
                              viewState === DatabaseManagerWindow.ViewState.Loading ? "⏳ Cargando..." :
                              viewState === DatabaseManagerWindow.ViewState.DataLoaded ? "✓ Datos Cargados" : ""
                        font.pixelSize: 12
                        font.bold: true
                    }
                }
            }
        }

        // ============ ÁREA PRINCIPAL ============
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 15

            // ---- TABLA DE DATOS ----
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "white"
                border.color: "#bdc3c7"
                border.width: 1
                radius: 5

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    // Encabezado con información
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 35
                        color: "#ecf0f1"
                        radius: 3

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 10

                            Text {
                                text: "📋 " + (dbManager.currentTable || "Selecciona una tabla")
                                font.pixelSize: 13
                                font.bold: true
                                color: "#2c3e50"
                            }

                            Item { Layout.fillWidth: true }

                            Text {
                                text: "Registros: " + dbManager.getRowCount()
                                font.pixelSize: 12
                                color: "#555"
                            }
                        }
                    }

                    // Tabla de datos
                    TableView {
                        id: dataTableView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: dbManager.tableModel
                        alternatingRowColors: true

                        delegate: Rectangle {
                            implicitWidth: 120
                            implicitHeight: 35
                            color: row === selectedRowIndex ? "#3498db" :
                                   index % 2 === 0 ? "#ffffff" : "#f8f9fa"

                            border.color: column === 0 ? "#bdc3c7" : "transparent"
                            border.width: 1

                            TextInput {
                                anchors.fill: parent
                                anchors.margins: 3
                                text: display ?? ""
                                color: row === selectedRowIndex ? "white" : "#2c3e50"
                                selectByMouse: true
                                readOnly: false

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
                                    onPressed: {
                                        selectedRowIndex = row
                                        mouse.accepted = false
                                    }
                                }
                            }
                        }
                    }

                    // Buscador / Filtro
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 40
                        color: "#ecf0f1"
                        radius: 3

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 8

                            Text {
                                text: "🔍"
                                font.pixelSize: 14
                            }

                            TextField {
                                id: searchField
                                Layout.fillWidth: true
                                placeholderText: "Filtrar datos..."
                                background: Rectangle {
                                    color: "white"
                                    border.color: "#bdc3c7"
                                    border.width: 1
                                    radius: 3
                                }
                            }

                            Button {
                                text: "Buscar"
                                Layout.preferredWidth: 80
                                onClicked: {
                                    if (searchField.text) {
                                        dbManager.filterTable(searchField.text)
                                    } else {
                                        dbManager.clearFilter()
                                    }
                                }

                                background: Rectangle {
                                    color: "#3498db"
                                    radius: 3
                                    border.color: "#2980b9"
                                    border.width: 1
                                }

                                contentItem: Text {
                                    text: parent.text
                                    color: "white"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                    font.bold: true
                                }
                            }

                            Button {
                                text: "Limpiar"
                                Layout.preferredWidth: 80
                                onClicked: {
                                    searchField.text = ""
                                    dbManager.clearFilter()
                                }

                                background: Rectangle {
                                    color: "#95a5a6"
                                    radius: 3
                                }

                                contentItem: Text {
                                    text: parent.text
                                    color: "white"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                    font.bold: true
                                }
                            }
                        }
                    }
                }
            }

            // ---- PANEL LATERAL DE CONTROLES ----
            Rectangle {
                Layout.preferredWidth: 200
                Layout.fillHeight: true
                color: "#ecf0f1"
                radius: 5
                border.color: "#bdc3c7"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 12

                    Text {
                        text: "🎮 CONTROLES"
                        font.pixelSize: 14
                        font.bold: true
                        color: "#2c3e50"
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1
                        color: "#bdc3c7"
                    }

                    // Botón Agregar
                    Button {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 45
                        text: "+ AGREGAR"

                        onClicked: {
                            if (!dbManager.addRow()) {
                                showError("No se pudo agregar registro")
                            }
                        }

                        background: Rectangle {
                            color: "#27ae60"
                            radius: 3
                            border.color: "#229954"
                            border.width: 1
                        }

                        contentItem: Text {
                            text: parent.text
                            color: "white"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    // Botón Eliminar
                    Button {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 45
                        text: "🗑 ELIMINAR"
                        enabled: selectedRowIndex >= 0

                        onClicked: {
                            if (!dbManager.removeRow(selectedRowIndex)) {
                                showError("No se pudo eliminar registro")
                            } else {
                                selectedRowIndex = -1
                            }
                        }

                        background: Rectangle {
                            color: enabled ? "#e74c3c" : "#bdc3c7"
                            radius: 3
                            border.color: enabled ? "#c0392b" : "#95a5a6"
                            border.width: 1
                        }

                        contentItem: Text {
                            text: parent.text
                            color: "white"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    // Botón Guardar
                    Button {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 45
                        text: "💾 GUARDAR"

                        onClicked: {
                            if (dbManager.submitChanges()) {
                                viewState = DatabaseManagerWindow.ViewState.DataLoaded
                            } else {
                                viewState = DatabaseManagerWindow.ViewState.Error
                            }
                        }

                        background: Rectangle {
                            color: "#3498db"
                            radius: 3
                            border.color: "#2980b9"
                            border.width: 1
                        }

                        contentItem: Text {
                            text: parent.text
                            color: "white"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    // Botón Revertir
                    Button {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 45
                        text: "↶ REVERTIR"

                        onClicked: {
                            dbManager.revertChanges()
                        }

                        background: Rectangle {
                            color: "#f39c12"
                            radius: 3
                            border.color: "#d68910"
                            border.width: 1
                        }

                        contentItem: Text {
                            text: parent.text
                            color: "white"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1
                        color: "#bdc3c7"
                    }

                    // Panel de información
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "#ffffff"
                        radius: 3
                        border.color: "#bdc3c7"
                        border.width: 1

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 8

                            Text {
                                text: "ℹ️ INFO"
                                font.pixelSize: 11
                                font.bold: true
                                color: "#2c3e50"
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 1
                                color: "#ecf0f1"
                            }

                            Text {
                                text: "Tabla:\n" + (dbManager.currentTable || "-")
                                font.pixelSize: 10
                                color: "#555"
                                wrapText: true
                            }

                            Text {
                                text: "Registros:\n" + dbManager.getRowCount()
                                font.pixelSize: 10
                                color: "#555"
                            }

                            Item { Layout.fillHeight: true }

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 50
                                color: "#ffebee"
                                radius: 3
                                visible: dbManager.lastError !== ""

                                ScrollView {
                                    anchors.fill: parent
                                    anchors.margins: 5

                                    Text {
                                        text: dbManager.lastError
                                        font.pixelSize: 9
                                        color: "#c62828"
                                        wrapText: true
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // ============ DIÁLOGO DE ERROR ============
    Dialog {
        id: errorDialog
        title: "Error"
        standardButtons: Dialog.Ok

        Text {
            text: errorDialog.errorMessage || "Ocurrió un error"
            wrapText: true
        }
    }

    function showError(message) {
        errorDialog.errorMessage = message
        errorDialog.open()
    }

    // Inicialización
    Component.onCompleted: {
        console.log("DatabaseManagerWindow inicializado")
        // Aquí se debería inicializar la BD con:
        // dbManager.initializeDatabase("/path/to/database.db")
    }

    Connections {
        target: dbManager
        function onErrorOccurred(errorMessage) {
            viewState = DatabaseManagerWindow.ViewState.Error
        }
        function onTableModelChanged() {
            viewState = DatabaseManagerWindow.ViewState.DataLoaded
            selectedRowIndex = -1
        }
        function onDataChanged() {
            viewState = DatabaseManagerWindow.ViewState.DataLoaded
        }
    }
}