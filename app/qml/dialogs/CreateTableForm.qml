import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    width: 900
    height: 700

    // =====================================================================
    // PROPIEDADES EXPUESTAS (ALIAS)
    // =====================================================================
    property alias dbNameFieldText: dbNameField.text
    property alias dbPathFieldText: dbPathField.text
    property alias tableNameText: tableNameField.text
    property alias fieldsModel: fieldsRepeater.model

    // Propiedades para los mensajes
    property alias messageTextContent: messageText.text
    property string messageType: "info"

    // =====================================================================
    // SEÑALES HACIA LA LÓGICA
    // =====================================================================
    signal showDbInfoRequested()
    signal addFieldRequested()
    signal moveFieldUpRequested(int index)
    signal moveFieldDownRequested(int index)
    signal removeFieldRequested(int index)

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15

        // =====================================================================
        // SECCIÓN 1: INFO DE LA BD
        // =====================================================================
        GroupBox {
            id: dbInfoGroup
            title: "Información de Base de Datos"
            Layout.fillWidth: true
            Layout.preferredHeight: 100

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 8

                RowLayout {
                    spacing: 15

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 5

                        Text {
                            text: "Nombre de BD:"
                            font.bold: true
                            color: "#333"
                        }
                        TextField {
                            id: dbNameField
                            placeholderText: "Ej: miproyecto"
                            readOnly: true
                            Layout.fillWidth: true
                            background: Rectangle {
                                border.color: "#ddd"
                                border.width: 1
                                radius: 4
                                color: "#f5f5f5"
                            }
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 5

                        Text {
                            text: "Ubicación:"
                            font.bold: true
                            color: "#333"
                        }
                        TextField {
                            id: dbPathField
                            placeholderText: "Ruta de almacenamiento"
                            readOnly: true
                            Layout.fillWidth: true
                            background: Rectangle {
                                border.color: "#ddd"
                                border.width: 1
                                radius: 4
                                color: "#f5f5f5"
                            }
                        }
                    }
                }

                Button {
                    text: "Ver Información Completa de BD"
                    Layout.alignment: Qt.AlignRight
                    onClicked: root.showDbInfoRequested()
                }
            }
        }

        // =====================================================================
        // SECCIÓN 2: NOMBRE DE TABLA
        // =====================================================================
        GroupBox {
            id: tableNameGroup
            title: "Nombre de la Tabla"
            Layout.fillWidth: true
            Layout.preferredHeight: 80

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 5

                    Text {
                        text: "Nombre de la tabla (solo letras, números, guiones bajos):"
                        font.pointSize: 10
                        color: "#555"
                    }
                    TextField {
                        id: tableNameField
                        placeholderText: "Ej: usuarios, productos, clientes"
                        Layout.fillWidth: true
                        background: Rectangle {
                            border.color: tableNameField.activeFocus ? "#4CAF50" : "#ddd"
                            border.width: 2
                            radius: 4
                            color: "#fff"
                        }
                    }
                }
            }
        }

        // =====================================================================
        // SECCIÓN 3: DEFINICIÓN DE CAMPOS
        // =====================================================================
        GroupBox {
            id: fieldsGroup
            title: "Definición de Campos"
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                Rectangle {
                    id: headerRow
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    color: "#f0f0f0"
                    border.color: "#ddd"
                    border.width: 1
                    radius: 4

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 10

                        Text { text: "Nombre del Campo"; font.bold: true; Layout.preferredWidth: 200; Layout.fillHeight: true; verticalAlignment: Text.AlignVCenter }
                        Text { text: "Tipo de Dato"; font.bold: true; Layout.preferredWidth: 120; Layout.fillHeight: true; verticalAlignment: Text.AlignVCenter }
                        Text { text: "Tamaño (solo TEXT)"; font.bold: true; Layout.preferredWidth: 130; Layout.fillHeight: true; verticalAlignment: Text.AlignVCenter }
                        Text { text: "Acciones"; font.bold: true; Layout.fillWidth: true; Layout.fillHeight: true; verticalAlignment: Text.AlignVCenter }
                    }
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ColumnLayout {
                        id: fieldsContainer
                        width: fieldsGroup.width - 20
                        spacing: 8

                        Repeater {
                            id: fieldsRepeater

                            delegate: Rectangle {
                                id: fieldRow
                                Layout.fillWidth: true
                                Layout.preferredHeight: 50
                                border.color: "#e0e0e0"
                                border.width: 1
                                radius: 4
                                color: index % 2 === 0 ? "#fafafa" : "#fff"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 10

                                    TextField {
                                        id: fieldNameInput
                                        placeholderText: "Ej: id, nombre, email"
                                        text: model.fieldName
                                        Layout.preferredWidth: 200
                                        onTextChanged: model.fieldName = text
                                        background: Rectangle { border.color: fieldNameInput.activeFocus ? "#4CAF50" : "#ddd"; border.width: 1; radius: 3; color: "#fff" }
                                    }

                                    ComboBox {
                                        id: typeCombo
                                        model: ["INT", "TEXT", "REAL", "DATE", "BOOLEAN", "BLOB"]
                                        currentIndex: find(model.fieldType)
                                        Layout.preferredWidth: 120
                                        onCurrentTextChanged: model.fieldType = currentText
                                        background: Rectangle { border.color: "#ddd"; border.width: 1; radius: 3; color: "#fff" }
                                    }

                                    TextField {
                                        id: sizeInput
                                        placeholderText: "Ej: 100, 255"
                                        text: model.fieldSize
                                        enabled: typeCombo.currentText === "TEXT"
                                        Layout.preferredWidth: 130
                                        inputMethodHints: Qt.ImhDigitsOnly
                                        onTextChanged: model.fieldSize = text
                                        background: Rectangle { border.color: sizeInput.activeFocus ? "#4CAF50" : "#ddd"; border.width: 1; radius: 3; color: enabled ? "#fff" : "#f5f5f5" }
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 5

                                        Button {
                                            text: "⬆"
                                            Layout.preferredWidth: 35
                                            Layout.preferredHeight: 35
                                            enabled: index > 0
                                            onClicked: root.moveFieldUpRequested(index)
                                            background: Rectangle { color: parent.hovered ? "#4CAF50" : "#e0e0e0"; radius: 3 }
                                        }

                                        Button {
                                            text: "⬇"
                                            Layout.preferredWidth: 35
                                            Layout.preferredHeight: 35
                                            enabled: index < fieldsRepeater.count - 1
                                            onClicked: root.moveFieldDownRequested(index)
                                            background: Rectangle { color: parent.hovered ? "#4CAF50" : "#e0e0e0"; radius: 3 }
                                        }

                                        Button {
                                            text: "✕"
                                            Layout.preferredWidth: 35
                                            Layout.preferredHeight: 35
                                            onClicked: root.removeFieldRequested(index)
                                            background: Rectangle { color: parent.hovered ? "#f44336" : "#e0e0e0"; radius: 3 }
                                        }

                                        Item { Layout.fillWidth: true }
                                    }
                                }
                            }
                        }
                    }
                }

                Button {
                    id: addFieldButton
                    text: "+ Agregar Campo"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    onClicked: root.addFieldRequested()

                    background: Rectangle { color: addFieldButton.hovered ? "#45a049" : "#4CAF50"; radius: 4 }
                    contentItem: Text {
                        text: addFieldButton.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                        font.pointSize: 11
                    }
                }
            }
        }

        // =====================================================================
        // SECCIÓN 4: MENSAJES
        // =====================================================================
        Rectangle {
            id: messageBox
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: root.messageType === "error" ? "#ffebee" : "#e3f2fd"
            border.color: root.messageType === "error" ? "#ef9a9a" : "#90caf9"
            border.width: 1
            radius: 4
            visible: messageText.text !== ""

            Text {
                id: messageText
                anchors.fill: parent
                anchors.margins: 10
                wrapMode: Text.WordWrap
                color: root.messageType === "error" ? "#d32f2f" : "#1976d2"
                font.pointSize: 10
            }
        }
    }
}