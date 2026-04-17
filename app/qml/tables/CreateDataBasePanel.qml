import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Item {
    id: root

    // Propiedades
    property var dbManager: null
    property int panelHeight: 0

    // Señales
    signal closed()
    signal databaseCreated(string dbName, string dbPath)

    width: parent?.width ?? 0
    height: 600  // Altura del panel

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 12

        // Header con botón de cerrar
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "Crear Nueva Base de Datos"
                font.bold: true
                font.pointSize: 12
                Layout.fillWidth: true
            }

            Button {
                text: "✕"
                implicitWidth: 40
                implicitHeight: 40
                onClicked: root.closed()
                background: Rectangle {
                    color: parent.hovered ? "#f44336" : "#e0e0e0"
                    radius: 4
                }
            }
        }

        // Campos de entrada
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 10

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 5

                Text {
                    text: "Nombre de la Base de Datos:"
                    font.bold: true
                    font.pointSize: 10
                }
                Rectangle {
                    Layout.fillWidth: true
                    height: 40
                    color: "lightgrey"
                    radius: 4

                    TextField {
                        id: dbNameInput
                        anchors.fill: parent
                        anchors.margins: 5
                        font.pointSize: 10
                        placeholderText: "Ej: miproyecto"
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 5

                Text {
                    text: "Ubicación (opcional, Enter para predeterminada):"
                    font.bold: true
                    font.pointSize: 10
                }
                Rectangle {
                    Layout.fillWidth: true
                    height: 40
                    color: "lightgrey"
                    radius: 4

                    TextField {
                        id: dbPathInput
                        anchors.fill: parent
                        anchors.margins: 5
                        font.pointSize: 10
                        placeholderText: "Ej: E:/Mis Documentos/"
                    }
                }
            }
        }

        // Mensajes
        Text {
            id: successMessage
            text: ""
            color: "#4CAF50"
            visible: text !== ""
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        Text {
            id: errorMessage
            text: ""
            color: "#d32f2f"
            visible: text !== ""
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        Item { Layout.fillHeight: true }

        // Botones
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Button {
                text: "Crear"
                Layout.fillWidth: true
                implicitHeight: 40
                font.pointSize: 10

                background: Rectangle {
                    color: parent.hovered ? "#45a049" : "#4CAF50"
                    radius: 4
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: createDatabase()
            }

            Button {
                text: "Cancelar"
                Layout.fillWidth: true
                implicitHeight: 40
                font.pointSize: 10

                background: Rectangle {
                    color: parent.hovered ? "#757575" : "#bdbdbd"
                    radius: 4
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: root.closed()
            }
        }
    }

    Timer {
        id: successTimer
        interval: 2000
        onTriggered: root.closed()
    }

    function createDatabase() {
        if (dbNameInput.text.trim() === "") {
            errorMessage.text = "El nombre no puede estar vacío"
            return
        }

        // Construir ruta completa
        var dbPath = dbPathInput.text.trim()
        var dbName = dbNameInput.text.trim()

        // Si dbPath está vacío, usar ruta por defecto
        if (dbPath === "") {
            dbPath = "./"
        }

        // Asegurar que termina con separador
        if (!dbPath.endsWith("/") && !dbPath.endsWith("\\")) {
            dbPath += "/"
        }

        // Agregar el nombre del archivo con extensión .db
        var fullPath = dbPath + dbName + ".db"

        console.log("Intentando crear BD en: " + fullPath)

        if (dbManager && dbManager.initializeDatabase(fullPath)) {
            successMessage.text = "✓ Base de datos creada en:\n" + dbManager.databasePath
            errorMessage.text = ""
            dbNameInput.text = ""
            dbPathInput.text = ""
            root.databaseCreated(dbName, dbManager.databasePath)
            successTimer.start()
        } else {
            errorMessage.text = "Error: " + (dbManager ? dbManager.getLastError() : "DBManager no disponible")
            successMessage.text = ""
        }
    }

    Component.onCompleted: {
        console.log("CreateDatabasePanel cargado")
    }
}
