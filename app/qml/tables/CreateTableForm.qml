import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    // Remove fixed dimensions — the parent (Dialog) controls sizing
    // width: 900  (removed)
    // height: 220 (removed)

    // Mobile breakpoint: forms narrower than 480 logical pixels
    readonly property bool isMobile: width > 0 && width < 480

    // Adaptive spacing and margins
    readonly property real layoutMargin: isMobile ? 8 : 15
    readonly property real layoutSpacing: isMobile ? 8 : 15
    readonly property real innerMargin: isMobile ? 6 : 10
    readonly property real innerSpacing: isMobile ? 5 : 8

    // Adaptive font sizes
    readonly property real labelFontSize: isMobile ? 9 : 8
    readonly property real inputFontSize: isMobile ? 10 : 8
    readonly property real messageFontSize: isMobile ? 10 : 10

    // Minimum touch target height for mobile (44px), compact on desktop
    readonly property real inputHeight: isMobile ? 44 : 32
    readonly property real buttonMinHeight: isMobile ? 44 : 35

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
        anchors.margins: root.layoutMargin
        spacing: root.layoutSpacing

        // =====================================================================
        // SECCIÓN 1: INFO DE LA BD
        // =====================================================================
        GroupBox {
            id: dbInfoGroup
            title: "Información de Base de Datos"
            Layout.fillWidth: true
            Layout.preferredHeight: root.isMobile ? 140 : 100
            font.pointSize: root.labelFontSize

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: root.innerMargin
                spacing: root.innerSpacing

                // On mobile, stack the two fields vertically; on desktop use side by side
                GridLayout {
                    Layout.fillWidth: true
                    columns: root.isMobile ? 1 : 2
                    rowSpacing: root.innerSpacing
                    columnSpacing: 15

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: root.isMobile ? 3 : 5

                        Text {
                            text: "Nombre de BD:"
                            font.bold: true
                            font.pointSize: root.labelFontSize
                            color: "#333"
                        }
                        TextField {
                            id: dbNameField
                            placeholderText: "Ej: miproyecto"
                            readOnly: true
                            Layout.fillWidth: true
                            Layout.preferredHeight: root.inputHeight
                            font.pointSize: root.inputFontSize
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
                        spacing: root.isMobile ? 3 : 5

                        Text {
                            text: "Ubicación:"
                            font.pointSize: root.labelFontSize
                            font.bold: true
                            color: "#333"
                        }
                        TextField {
                            id: dbPathField
                            placeholderText: "Ruta de almacenamiento"
                            readOnly: true
                            font.pointSize: root.inputFontSize
                            Layout.fillWidth: true
                            Layout.preferredHeight: root.inputHeight
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
                    implicitHeight: root.buttonMinHeight
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
            Layout.preferredHeight: root.isMobile ? 100 : 80
            font.pointSize: root.labelFontSize

            RowLayout {
                anchors.fill: parent
                anchors.margins: root.innerMargin
                spacing: 0

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 0

                    Text {
                        text: "Nombre de la tabla (solo letras, números, guiones bajos):"
                        font.pointSize: root.labelFontSize
                        color: "#555"
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }
                    TextField {
                        id: tableNameField
                        placeholderText: "Ej: usuarios, productos, clientes"
                        Layout.fillWidth: true
                        Layout.preferredHeight: root.inputHeight
                        font.pointSize: root.inputFontSize
                        background: Rectangle {
                            border.color: tableNameField.activeFocus ? "#4CAF50" : "#ddd"
                            border.width: 1
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
            font.pointSize: root.labelFontSize

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: root.innerMargin
                spacing: root.innerSpacing

                // Header row — hidden on mobile to save space (labels appear inline in each row)
                Rectangle {
                    id: headerRow
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    color: "#f0f0f0"
                    border.color: "#ddd"
                    border.width: 1
                    radius: 4
                    visible: !root.isMobile

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 10

                        Text { text: "Nombre del Campo"; font.pointSize: root.labelFontSize; font.bold: true; Layout.fillWidth: true; Layout.fillHeight: true; verticalAlignment: Text.AlignVCenter }
                        Text { text: "Tipo de Dato"; font.pointSize: root.labelFontSize; font.bold: true; Layout.preferredWidth: 110; Layout.fillHeight: true; verticalAlignment: Text.AlignVCenter }
                        Text { text: "Tamaño (solo TEXT)"; font.pointSize: root.labelFontSize; font.bold: true; Layout.preferredWidth: 120; Layout.fillHeight: true; verticalAlignment: Text.AlignVCenter }
                        Text { text: "Acciones"; font.pointSize: root.labelFontSize; font.bold: true; Layout.preferredWidth: 120; Layout.fillHeight: true; verticalAlignment: Text.AlignVCenter }
                    }
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ColumnLayout {
                        id: fieldsContainer
                        // Subtract horizontal margins applied on both sides inside the GroupBox
                        width: fieldsGroup.width - root.innerMargin * 2
                        spacing: root.innerSpacing

                        Repeater {
                            id: fieldsRepeater

                            delegate: Rectangle {
                                id: fieldRow
                                Layout.fillWidth: true
                                // Mobile: two stacked input rows + margins; desktop: single row
                                Layout.preferredHeight: root.isMobile
                                    ? (root.inputHeight * 2 + 4 + root.innerMargin * 2)
                                    : 50
                                border.color: "#e0e0e0"
                                border.width: 1
                                radius: 4
                                color: index % 2 === 0 ? "#fafafa" : "#fff"

                                // Desktop: single-row layout
                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 10
                                    visible: !root.isMobile

                                    TextField {
                                        id: fieldNameInput
                                        placeholderText: "Ej: id, nombre, email"
                                        text: model.fieldName
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: root.inputHeight
                                        font.pointSize: root.inputFontSize
                                        onTextChanged: model.fieldName = text
                                        background: Rectangle { border.color: fieldNameInput.activeFocus ? "#4CAF50" : "#ddd"; border.width: 1; radius: 3; color: "#fff" }
                                    }

                                    ComboBox {
                                        id: typeCombo
                                        model: ["INT", "TEXT", "REAL", "DATE", "BOOLEAN", "BLOB"]
                                        currentIndex: find(model.fieldType)
                                        Layout.preferredWidth: 110
                                        Layout.preferredHeight: root.inputHeight
                                        onCurrentTextChanged: model.fieldType = currentText
                                        background: Rectangle { border.color: "#ddd"; border.width: 1; radius: 3; color: "#fff" }
                                    }

                                    TextField {
                                        id: sizeInput
                                        placeholderText: "Ej: 100, 255"
                                        text: model.fieldSize
                                        enabled: typeCombo.currentText === "TEXT"
                                        Layout.preferredWidth: 120
                                        Layout.preferredHeight: root.inputHeight
                                        inputMethodHints: Qt.ImhDigitsOnly
                                        font.pointSize: root.inputFontSize
                                        onTextChanged: model.fieldSize = text
                                        background: Rectangle { border.color: sizeInput.activeFocus ? "#4CAF50" : "#ddd"; border.width: 1; radius: 3; color: enabled ? "#fff" : "#f5f5f5" }
                                    }

                                    RowLayout {
                                        spacing: 5

                                        Button {
                                            text: "⬆"
                                            implicitWidth: root.buttonMinHeight
                                            implicitHeight: root.buttonMinHeight
                                            enabled: index > 0
                                            onClicked: root.moveFieldUpRequested(index)
                                            background: Rectangle { color: parent.hovered ? "#4CAF50" : "#e0e0e0"; radius: 3 }
                                        }

                                        Button {
                                            text: "⬇"
                                            implicitWidth: root.buttonMinHeight
                                            implicitHeight: root.buttonMinHeight
                                            enabled: index < fieldsRepeater.count - 1
                                            onClicked: root.moveFieldDownRequested(index)
                                            background: Rectangle { color: parent.hovered ? "#4CAF50" : "#e0e0e0"; radius: 3 }
                                        }

                                        Button {
                                            text: "✕"
                                            implicitWidth: root.buttonMinHeight
                                            implicitHeight: root.buttonMinHeight
                                            onClicked: root.removeFieldRequested(index)
                                            background: Rectangle { color: parent.hovered ? "#f44336" : "#e0e0e0"; radius: 3 }
                                        }
                                    }
                                }

                                // Mobile: stacked/card layout
                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 4
                                    visible: root.isMobile

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 6

                                        TextField {
                                            id: fieldNameInputMobile
                                            placeholderText: "Nombre campo"
                                            text: model.fieldName
                                            Layout.fillWidth: true
                                            Layout.preferredHeight: root.inputHeight
                                            font.pointSize: root.inputFontSize
                                            onTextChanged: model.fieldName = text
                                            background: Rectangle { border.color: fieldNameInputMobile.activeFocus ? "#4CAF50" : "#ddd"; border.width: 1; radius: 3; color: "#fff" }
                                        }

                                        ComboBox {
                                            id: typeComboMobile
                                            model: ["INT", "TEXT", "REAL", "DATE", "BOOLEAN", "BLOB"]
                                            currentIndex: find(model.fieldType)
                                            Layout.preferredWidth: 100
                                            Layout.preferredHeight: root.inputHeight
                                            onCurrentTextChanged: model.fieldType = currentText
                                            background: Rectangle { border.color: "#ddd"; border.width: 1; radius: 3; color: "#fff" }
                                        }
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 6

                                        TextField {
                                            id: sizeInputMobile
                                            placeholderText: "Tamaño (solo TEXT)"
                                            text: model.fieldSize
                                            enabled: typeComboMobile.currentText === "TEXT"
                                            Layout.fillWidth: true
                                            Layout.preferredHeight: root.inputHeight
                                            inputMethodHints: Qt.ImhDigitsOnly
                                            font.pointSize: root.inputFontSize
                                            onTextChanged: model.fieldSize = text
                                            background: Rectangle { border.color: sizeInputMobile.activeFocus ? "#4CAF50" : "#ddd"; border.width: 1; radius: 3; color: enabled ? "#fff" : "#f5f5f5" }
                                        }

                                        Button {
                                            text: "⬆"
                                            implicitWidth: root.buttonMinHeight
                                            implicitHeight: root.buttonMinHeight
                                            enabled: index > 0
                                            onClicked: root.moveFieldUpRequested(index)
                                            background: Rectangle { color: parent.hovered ? "#4CAF50" : "#e0e0e0"; radius: 3 }
                                        }

                                        Button {
                                            text: "⬇"
                                            implicitWidth: root.buttonMinHeight
                                            implicitHeight: root.buttonMinHeight
                                            enabled: index < fieldsRepeater.count - 1
                                            onClicked: root.moveFieldDownRequested(index)
                                            background: Rectangle { color: parent.hovered ? "#4CAF50" : "#e0e0e0"; radius: 3 }
                                        }

                                        Button {
                                            text: "✕"
                                            implicitWidth: root.buttonMinHeight
                                            implicitHeight: root.buttonMinHeight
                                            onClicked: root.removeFieldRequested(index)
                                            background: Rectangle { color: parent.hovered ? "#f44336" : "#e0e0e0"; radius: 3 }
                                        }
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
                    Layout.preferredHeight: root.buttonMinHeight
                    onClicked: root.addFieldRequested()

                    background: Rectangle { color: addFieldButton.hovered ? "#45a049" : "#4CAF50"; radius: 4 }
                    contentItem: Text {
                        text: addFieldButton.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                        font.pointSize: root.inputFontSize
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
            Layout.preferredHeight: root.isMobile ? 70 : 60
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
                font.pointSize: root.messageFontSize
            }
        }
    }
}