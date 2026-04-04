import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Dialog {
    id: createTableDialog
    title: "Crear Nueva Tabla"
    width: 900
    height: 700
    modal: true
    standardButtons: Dialog.Cancel | Dialog.Ok

    // Propiedades expuestas
    property var dbManager: null
    property string currentDatabaseName: ""
    property string currentDatabasePath: ""

    // Funciones de callback
    signal tableCreationRequested(string tableName, var fields)

    onAccepted: {
        if (validateInputs()) {
            createTableAction()
        }
    }

    // =====================================================================
    // CONTENIDO PRINCIPAL
    // =====================================================================
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
                            text: createTableDialog.currentDatabaseName
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
                            text: createTableDialog.currentDatabasePath
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

                // Botón para ver información completa
                Button {
                    text: "Ver Información Completa de BD"
                    Layout.alignment: Qt.AlignRight
                    onClicked: {
                        dbInfoDialogPopup.infoText = dbManager.getDatabaseInfo()
                        dbInfoDialogPopup.open()
                    }
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

                    Text {
                        id: tableNameError
                        text: ""
                        color: "#d32f2f"
                        font.pointSize: 9
                        visible: text !== ""
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

                // Encabezados de columnas
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

                        Text {
                            text: "Nombre del Campo"
                            font.bold: true
                            Layout.preferredWidth: 200
                            Layout.fillHeight: true
                            verticalAlignment: Text.AlignVCenter
                        }
                        Text {
                            text: "Tipo de Dato"
                            font.bold: true
                            Layout.preferredWidth: 120
                            Layout.fillHeight: true
                            verticalAlignment: Text.AlignVCenter
                        }
                        Text {
                            text: "Tamaño (solo TEXT)"
                            font.bold: true
                            Layout.preferredWidth: 130
                            Layout.fillHeight: true
                            verticalAlignment: Text.AlignVCenter
                        }
                        Text {
                            text: "Acciones"
                            font.bold: true
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

                // Lista de campos
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
                            model: fieldsListModel

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

                                    // Campo: Nombre
                                    TextField {
                                        id: fieldNameInput
                                        placeholderText: "Ej: id, nombre, email"
                                        text: model.fieldName
                                        Layout.preferredWidth: 200
                                        onTextChanged: model.fieldName = text

                                        background: Rectangle {
                                            border.color: fieldNameInput.activeFocus ? "#4CAF50" : "#ddd"
                                            border.width: 1
                                            radius: 3
                                            color: "#fff"
                                        }
                                    }

                                    // Campo: Tipo
                                    ComboBox {
                                        id: typeCombo
                                        model: ["INT", "TEXT", "REAL", "DATE", "BOOLEAN", "BLOB"]
                                        currentIndex: find(model.fieldType)
                                        Layout.preferredWidth: 120
                                        onCurrentTextChanged: model.fieldType = currentText

                                        background: Rectangle {
                                            border.color: "#ddd"
                                            border.width: 1
                                            radius: 3
                                            color: "#fff"
                                        }
                                    }

                                    // Campo: Tamaño
                                    TextField {
                                        id: sizeInput
                                        placeholderText: "Ej: 100, 255"
                                        text: model.fieldSize
                                        enabled: typeCombo.currentText === "TEXT"
                                        Layout.preferredWidth: 130
                                        inputMethodHints: Qt.ImhDigitsOnly
                                        onTextChanged: model.fieldSize = text

                                        background: Rectangle {
                                            border.color: sizeInput.activeFocus ? "#4CAF50" : "#ddd"
                                            border.width: 1
                                            radius: 3
                                            color: enabled ? "#fff" : "#f5f5f5"
                                        }
                                    }

                                    // Botones de acción
                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 5

                                        Button {
                                            text: "⬆"
                                            Layout.preferredWidth: 35
                                            Layout.preferredHeight: 35
                                            enabled: index > 0
                                            onClicked: fieldsListModel.moveField(index, index - 1)

                                            background: Rectangle {
                                                color: parent.hovered ? "#4CAF50" : "#e0e0e0"
                                                radius: 3
                                            }
                                        }

                                        Button {
                                            text: "⬇"
                                            Layout.preferredWidth: 35
                                            Layout.preferredHeight: 35
                                            enabled: index < fieldsRepeater.count - 1
                                            onClicked: fieldsListModel.moveField(index, index + 1)

                                            background: Rectangle {
                                                color: parent.hovered ? "#4CAF50" : "#e0e0e0"
                                                radius: 3
                                            }
                                        }

                                        Button {
                                            text: "✕"
                                            Layout.preferredWidth: 35
                                            Layout.preferredHeight: 35
                                            onClicked: fieldsListModel.removeField(index)

                                            background: Rectangle {
                                                color: parent.hovered ? "#f44336" : "#e0e0e0"
                                                radius: 3
                                            }
                                        }

                                        Item { Layout.fillWidth: true }
                                    }
                                }
                            }
                        }

                        Item { Layout.fillHeight: true }
                    }
                }

                // Botón para agregar campo
                Button {
                    id: addFieldButton
                    text: "+ Agregar Campo"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40

                    onClicked: fieldsListModel.addField()

                    background: Rectangle {
                        color: addFieldButton.hovered ? "#45a049" : "#4CAF50"
                        radius: 4
                    }

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
            color: "#f0f0f0"
            border.color: "#ddd"
            border.width: 1
            radius: 4
            visible: messageText.text !== ""

            Text {
                id: messageText
                anchors.fill: parent
                anchors.margins: 10
                wrapMode: Text.WordWrap
                color: messageType === "error" ? "#d32f2f" : "#1976d2"
                font.pointSize: 10
            }
        }
    }

    // =====================================================================
    // MODELOS Y LÓGICA
    // =====================================================================

    ListModel {
        id: fieldsListModel

        function addField() {
            append({
                fieldName: "",
                fieldType: "TEXT",
                fieldSize: ""
            })
        }

        function removeField(index) {
            remove(index)
        }

        function moveField(fromIndex, toIndex) {
            if (toIndex >= 0 && toIndex < count) {
                move(fromIndex, toIndex, 1)
            }
        }

        function clear() {
            fieldsListModel.clear()
        }

        function getFieldsList() {
            var fields = []
            for (var i = 0; i < count; i++) {
                fields.push({
                    name: get(i).fieldName,
                    type: get(i).fieldType,
                    size: get(i).fieldSize
                })
            }
            return fields
        }
    }

    property string messageType: "info"

    function showMessage(message, type = "info") {
        messageText.text = message
        messageType = type
    }

    function clearMessage() {
        messageText.text = ""
    }

    function validateInputs() {
        clearMessage()

        // Validar nombre de tabla
        if (tableNameField.text.trim() === "") {
            showMessage("❌ El nombre de la tabla no puede estar vacío", "error")
            return false
        }

        // Validar que sea un nombre válido (solo letras, números, guiones bajos)
        var validName = /^[a-zA-Z_][a-zA-Z0-9_]*$/.test(tableNameField.text)
        if (!validName) {
            showMessage("❌ El nombre de la tabla solo puede contener letras, números y guiones bajos, y debe empezar con letra o guion bajo", "error")
            return false
        }

        // Validar que la tabla no exista
        if (dbManager.tableExists(tableNameField.text)) {
            showMessage("❌ La tabla '" + tableNameField.text + "' ya existe en la base de datos", "error")
            return false
        }

        // Validar campos
        if (fieldsListModel.count === 0) {
            showMessage("❌ Debe agregar al menos un campo", "error")
            return false
        }

        // Validar cada campo
        for (var i = 0; i < fieldsListModel.count; i++) {
            var field = fieldsListModel.get(i)

            if (field.fieldName.trim() === "") {
                showMessage("❌ El campo " + (i + 1) + " no tiene nombre", "error")
                return false
            }

            var validFieldName = /^[a-zA-Z_][a-zA-Z0-9_]*$/.test(field.fieldName)
            if (!validFieldName) {
                showMessage("❌ El campo '" + field.fieldName + "' tiene un nombre inválido", "error")
                return false
            }

            if (field.fieldType === "TEXT" && field.fieldSize.trim() === "") {
                showMessage("⚠ El campo '" + field.fieldName + "' (TEXT) no tiene tamaño especificado. Se usará el máximo", "info")
            }
        }

        return true
    }

    function createTableAction() {
        if (!dbManager) {
            showMessage("❌ DBManager no está configurado", "error")
            return
        }

        var fields = fieldsListModel.getFieldsList()
        var tableName = tableNameField.text.trim()

        if (dbManager.createTable(tableName, fields)) {
            showMessage("✓ Tabla '" + tableName + "' creada exitosamente", "info")

            // Esperar un momento y luego cerrar el diálogo
            closeTimer.start()
        } else {
            showMessage("❌ Error: " + dbManager.getLastError(), "error")
        }
    }

    Timer {
        id: closeTimer
        interval: 2000
        onTriggered: {
            createTableDialog.close()
        }
    }

    // =====================================================================
    // DIÁLOGO INFO DE BD
    // =====================================================================
    Dialog {
        id: dbInfoDialogPopup
        title: "Información de Base de Datos"
        width: 700
        height: 500
        modal: true

        property string infoText: ""

        ScrollView {
            anchors.fill: parent
            clip: true

            Text {
                text: dbInfoDialogPopup.infoText
                wrapMode: Text.Wrap
                padding: 15
                font.family: "Courier New"
                font.pointSize: 9
                color: "#333"
            }
        }

        standardButtons: Dialog.Close
    }

    // =====================================================================
    // INICIALIZACIÓN
    // =====================================================================
    Component.onCompleted: {
        // Agregar un campo inicial vacío
        fieldsListModel.addField()
    }

    onOpened: {
        tableNameField.text = ""
        tableNameField.forceActiveFocus()
        clearMessage()
    }
}