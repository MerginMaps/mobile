import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

import mm 1.0 as MM

import "../components"
import "../inputs"

MMPage {
    id: root

    pageHeader.title: qsTr( "Crear Base de datos" )
    // Propiedades
    property var dbManager: null
    property int panelHeight: 0

    // Señales
    signal closed()
    signal databaseCreated(string dbName, string dbPath)

    //width: parent?.width ?? 0
    //height: 600  // Altura del panel

    function back()
    {
      // close the last page; if there is only one, close the controller

      if (pagesStackView.depth > 1) {
        pagesStackView.pop( null )
      }
      else {
        pagesStackView.clear()
        root.closed()
      }
    }

    StackView {
      id: pagesStackView

      width: ApplicationWindow.window?.width ?? 0
      height: ApplicationWindow.window?.height ?? 0

     // anchors.fill: parent
    }
    onBackClicked: root.back()
    //Keys.onReleased: function( event ) {
    //  if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
    //    event.accepted = true

      //  if ( pagesStackView.depth === 1 ) {
      //    root.close()
      //  }
      //  else {
      //    pagesStackView.pop( StackView.PopTransition )
      //  }
     // }
   // }

pageContent: ScrollView {

  width: parent.width
  height: parent.height

  contentWidth: availableWidth // to only scroll vertically
  ScrollBar.vertical.policy: ScrollBar.AlwaysOff
  ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

  Component {
      id: databaseListPage

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 12

        // Header con botón de cerrar
       // RowLayout {
       //     Layout.fillWidth: true
         //   spacing: 10
//
//            MMText {
//                text: "Crear Nueva Base de Datos"
 //               font.bold: true
  //              font.pointSize: 12
 //               Layout.fillWidth: true
//            }

           // MMButton {
           //     width: parent.width
            //    anchors.bottom: parent.bottom
             //   anchors.bottomMargin: root.hasToolbar ? __style.margin20 : ( __style.safeAreaBottom + __style.margin8 )
            //    text: "✕"
  //        //      implicitWidth: 40
//          //      implicitHeight: 40
            //    onClicked: root.closed()
           //     background: Rectangle {
           //         color: parent.hovered ? "#f44336" : "#e0e0e0"
           //         radius: 4
            //    }
          //  }
       // }

        // Campos de entrada
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 10

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 5

                MMText {
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

                MMText {
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
        MMText {
            id: successMessage
            text: ""
            color: "#4CAF50"
            visible: text !== ""
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        MMText {
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
        let item = pagesStackView.push( databaseListPage, {}, StackView.Immediate )
        item.forceActiveFocus()

    }
 }
}
