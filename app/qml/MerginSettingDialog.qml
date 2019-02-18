import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick

Dialog {
    property string linkFieldText: __merginApi.apiRoot
    property real fieldHeight: InputStyle.rowHeight
    property color fontColor: InputStyle.fontColor
    property color bgColor: "white"
    property real panelMargin: InputStyle.panelMargin

    signal closing()

    id: merginLinkDialog
    modality: Qt.WindowModal
    title: "Set Mergin instance"

    onApply: {
        __merginApi.apiRoot = linkField.text
        merginLinkDialog.close()
        closing()
    }
    onRejected: {
        linkField.text = __merginApi.apiRoot
        merginLinkDialog.close()
        closing()
    }
    onReset: {
        __merginApi.resetApiRoot()
        merginLinkDialog.close()
        closing()
    }


    contentItem: Rectangle {
        id: dialogContent
        height: fieldHeight * 4
        width: window.width - 2 * merginLinkDialog.panelMargin

        Keys.onEscapePressed: merginLinkDialog.reject()
        Keys.onBackPressed: merginLinkDialog.reject()
        color: merginLinkDialog.bgColor

        Item {
            anchors.fill: parent
            anchors.margins: merginLinkDialog.panelMargin

            ColumnLayout {
                width: parent.width
                height: parent.height
                anchors.margins: merginLinkDialog.panelMargin

                RowLayout {
                    id: row
                    Layout.fillWidth: true
                    height: fieldHeight

                    Item {
                        id: iconContainer
                        height: fieldHeight/2
                        width: fieldHeight/2

                        Image {
                            anchors.margins: (fieldHeight/4)
                            id: icon
                            height: iconContainer.height
                            width: height
                            source: 'link.svg'
                            sourceSize.width: width
                            sourceSize.height: height
                            fillMode: Image.PreserveAspectFit
                        }

                        ColorOverlay {
                            anchors.fill: icon
                            source: icon
                            color: merginLinkDialog.fontColor
                        }
                    }

                    TextField {
                        id: linkField
                        text: merginLinkDialog.linkFieldText
                        height: fieldHeight
                        font.pixelSize: InputStyle.fontPixelSizeNormal
                        color: merginLinkDialog.fontColor
                        placeholderText: qsTr("Mergin instance")
                        inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
                        font.capitalization: Font.MixedCase
                        Layout.fillWidth: true
                        background: Rectangle {
                            color: merginLinkDialog.bgColor
                        }

                        onAccepted: merginLinkDialog.apply()
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    height: 2 * QgsQuick.Utils.dp

                    Rectangle {
                        id: loginNameBorder
                        color: merginLinkDialog.fontColor
                        height: 2 * QgsQuick.Utils.dp
                        opacity: linkField.focus ? 1 : 0.6
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    id: dialogButtons
                    width: parent.width
                    Button {
                        Layout.fillWidth: true
                        text: "Default"
                        font.bold: true
                        flat: true
                        background: Rectangle {
                            color: merginLinkDialog.bgColor
                        }
                        onClicked:merginLinkDialog.click(StandardButton.Reset)
                    }

                    Button {
                        Layout.fillWidth: true
                        text: "Cancel"
                        flat: true
                        font.bold: true
                        background: Rectangle {
                            color: merginLinkDialog.bgColor
                        }
                        onClicked: merginLinkDialog.click(StandardButton.Cancel)
                    }

                    Button {
                        Layout.fillWidth: true
                        text: "Apply"
                        flat: true
                        font.bold: true
                        background: Rectangle {
                            color: merginLinkDialog.bgColor
                        }
                        onClicked: merginLinkDialog.click(StandardButton.Apply)
                    }
                }
            }
        }
    }
}
