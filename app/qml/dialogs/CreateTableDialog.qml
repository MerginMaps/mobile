import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Dialog {
    id: createTableDialog
    title: "Crear Nueva Tabla"
    width: 700
    height: 700
    modal: true
//    standardButtons: Dialog.Cancel | Dialog.Ok
    footer: DialogButtonBox {
            Button {
                text: "Aceptar"
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                implicitWidth: 5 // Tamaño fijo
                implicitHeight: 20
                font.pointSize: 8
            }
            Button {
                text: "Cancelar"
                DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
                implicitWidth: 5
                implicitHeight: 20
                font.pointSize: 8
            }
        }


    // Propiedades expuestas
    property var dbManager: null
    property string currentDatabaseName: ""
    property string currentDatabasePath: ""

    // Funciones de callback
    signal tableCreationRequested(string tableName, var fields)

    // =====================================================================
    // INSTANCIA DE LA INTERFAZ VISUAL
    // =====================================================================
    CreateTableForm {
        id: uiForm
        anchors.fill: parent

        // 1. Enviar datos a la vista
        dbNameFieldText: createTableDialog.currentDatabaseName
        dbPathFieldText: createTableDialog.currentDatabasePath
        fieldsModel: fieldsListModel

        // 2. Recibir señales de la vista
        onShowDbInfoRequested: {
            if (dbManager) {
                dbInfoDialogPopup.infoText = dbManager.getDatabaseInfo()
                dbInfoDialogPopup.open()
            }
        }

        onAddFieldRequested: fieldsListModel.addField()
        onMoveFieldUpRequested: (index) => fieldsListModel.moveField(index, index - 1)
        onMoveFieldDownRequested: (index) => fieldsListModel.moveField(index, index + 1)
        onRemoveFieldRequested: (index) => fieldsListModel.removeField(index)
    }

    // =====================================================================
    // MODELOS Y LÓGICA
    // =====================================================================
    ListModel {
        id: fieldsListModel

        function addField() {
            append({ fieldName: "", fieldType: "TEXT", fieldSize: "" })
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

    // Funciones de validación y control
    function showMessage(message, type = "info") {
        uiForm.messageTextContent = message
        uiForm.messageType = type
    }

    function clearMessage() {
        uiForm.messageTextContent = ""
    }

    function validateInputs() {
        clearMessage()

        if (uiForm.tableNameText.trim() === "") {
            showMessage("❌ El nombre de la tabla no puede estar vacío", "error")
            return false
        }

        var validName = /^[a-zA-Z_][a-zA-Z0-9_]*$/.test(uiForm.tableNameText)
        if (!validName) {
            showMessage("❌ El nombre de la tabla solo puede contener letras, números y guiones bajos", "error")
            return false
        }

        if (dbManager && dbManager.tableExists(uiForm.tableNameText)) {
            showMessage("❌ La tabla '" + uiForm.tableNameText + "' ya existe", "error")
            return false
        }

        if (fieldsListModel.count === 0) {
            showMessage("❌ Debe agregar al menos un campo", "error")
            return false
        }

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
        }

        return true
    }

    function createTableAction() {
        if (!dbManager) {
            showMessage("❌ DBManager no está configurado", "error")
            return
        }

        var fields = fieldsListModel.getFieldsList()
        var tableName = uiForm.tableNameText.trim()

        if (dbManager.createTable(tableName, fields)) {
            showMessage("✓ Tabla '" + tableName + "' creada exitosamente", "info")
            closeTimer.start()
        } else {
            showMessage("❌ Error: " + dbManager.getLastError(), "error")
        }
    }

    // =====================================================================
    // EVENTOS Y EXTRAS DEL DIÁLOGO
    // =====================================================================
    onAccepted: {
        if (validateInputs()) {
            createTableAction()
        }
    }

    onOpened: {
        clearMessage()
    }

    Component.onCompleted: {
        fieldsListModel.addField()
    }

    Timer {
        id: closeTimer
        interval: 2000
        onTriggered: createTableDialog.close()
    }

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
}