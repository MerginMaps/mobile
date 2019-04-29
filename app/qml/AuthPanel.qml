import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Item {

    signal authFailed()

    property alias merginLink: merginLink
    property alias loginName: loginName
    property alias password: password
    property string errorText: errorText

    property real fieldHeight: InputStyle.rowHeight
    property real panelMargin: fieldHeight/4
    property color fontColor: InputStyle.panelBackgroundDark
    property color bgColor: "white"

    function close() {
        visible = false
        password.text = ""
        loginName.text = ""
        if (!__merginApi.hasAuthData()) {
            authFailed()
        }
    }

    Keys.onReleased: {
        if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
            event.accepted = true;
            root.close()
        }
    }

    id: root
    focus: true

    Pane {
        id: pane

        width: parent.width
        height: parent.height
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        background: Rectangle {
            color: root.bgColor
        }

        Item {
            id: loginForm
            anchors.fill: parent
            anchors.bottomMargin: Qt.inputMethod.keyboardRectangle.height ? Qt.inputMethod.keyboardRectangle.height: 0

            Column {
                id: columnLayout
                spacing: root.panelMargin
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    source: "mergin_color.svg"
                    width: loginForm.width/2
                    sourceSize.width: width
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Row {
                    id: row
                    width: loginForm.width
                    height: fieldHeight
                    spacing: 0

                    Rectangle {
                        id: iconContainer
                        height: fieldHeight
                        width: fieldHeight
                        color: root.bgColor

                        Image {
                            anchors.margins: root.panelMargin
                            id: icon
                            height: fieldHeight
                            width: fieldHeight
                            anchors.fill: parent
                            source: 'account.svg'
                            sourceSize.width: width
                            sourceSize.height: height
                            fillMode: Image.PreserveAspectFit
                        }

                        ColorOverlay {
                            anchors.fill: icon
                            source: icon
                            color: root.fontColor
                        }
                    }

                    TextField {
                        id: loginName
                        x: iconContainer.width
                        width: parent.width - iconContainer.width
                        height: fieldHeight
                        font.pixelSize: InputStyle.fontPixelSizeNormal
                        color: root.fontColor
                        placeholderText: qsTr("Username")
                        font.capitalization: Font.MixedCase
                        inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
                        background: Rectangle {
                            color: root.bgColor
                        }
                    }
                }

                Rectangle {
                    id: loginNameBorder
                    color: root.fontColor
                    y: loginName.height - height
                    height: 2 * QgsQuick.Utils.dp
                    opacity: loginName.focus ? 1 : 0.6
                    width: parent.width - fieldHeight/2
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Row {
                    width: loginForm.width
                    height: fieldHeight
                    spacing: 0

                    Rectangle {
                        id: iconContainer2
                        height: fieldHeight
                        width: fieldHeight
                        color: root.bgColor

                        Image {
                            anchors.margins: (fieldHeight/4)
                            id: icon2
                            height: fieldHeight
                            width: fieldHeight
                            anchors.fill: parent
                            source: 'lock.svg'
                            sourceSize.width: width
                            sourceSize.height: height
                            fillMode: Image.PreserveAspectFit

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (password.echoMode === TextInput.Normal) {
                                        password.echoMode = TextInput.Password
                                    } else {
                                        password.echoMode = TextInput.Normal
                                    }
                                }
                            }
                        }

                        ColorOverlay {
                            anchors.fill: icon2
                            source: icon2
                            color: root.fontColor
                        }
                    }

                    TextField {
                        id: password
                        width: parent.width - iconContainer.width
                        height: fieldHeight
                        font.pixelSize: InputStyle.fontPixelSizeNormal
                        color: root.fontColor
                        placeholderText: qsTr("Password")
                        echoMode: TextInput.Password
                        inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
                        font.capitalization: Font.MixedCase

                        background: Rectangle {
                            color: root.bgColor
                        }
                    }
                }

                Rectangle {
                    id: passBorder
                    color: InputStyle.panelBackgroundDark
                    height: 2 * QgsQuick.Utils.dp
                    y: password.height - height
                    opacity: password.focus ? 1 : 0.6
                    width: loginForm.width - fieldHeight/2
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Button {
                    id: loginButton
                    width: loginForm.width - 2* root.panelMargin
                    height: fieldHeight
                    text: qsTr("Sign in")
                    font.pixelSize: loginButton.height/2
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: {
                        __merginApi.authorize(loginName.text, password.text)
                    }
                    background: Rectangle {
                        color: InputStyle.highlightColor
                    }

                    contentItem: Text {
                        text: loginButton.text
                        font: loginButton.font
                        opacity: enabled ? 1.0 : 0.3
                        color: root.bgColor
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                }

                Button {
                    id: signUpButton
                    width: loginForm.width - 2* root.panelMargin
                    height: fieldHeight * 0.7
                    text: qsTr("Sign up")
                    font.pixelSize: signUpButton.height/2
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked:Qt.openUrlExternally(__merginApi.apiRoot);
                    background: Rectangle {
                        color: root.bgColor
                    }

                    contentItem: Text {
                        text: signUpButton.text
                        font: signUpButton.font
                        color: InputStyle.highlightColor
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                }

                Row {
                    height: fieldHeight
                    anchors.horizontalCenter: parent.horizontalCenter
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                    Item {
                        id: iconContainerMergin
                        height: fieldHeight/2
                        width: fieldHeight/2

                        MouseArea {
                            width: iconContainerMergin.width
                            height: iconContainerMergin.height
                            onClicked: {
                                merginDialog.open()
                            }
                        }

                        Image {
                            id: iconLink
                            anchors.fill: parent
                            anchors.margins: parent.height * 0.25
                            source: 'edit.svg'
                            sourceSize.width: width
                            sourceSize.height: height
                            fillMode: Image.PreserveAspectFit
                        }

                        ColorOverlay {
                            anchors.fill: iconLink
                            source: iconLink
                            color: InputStyle.panelBackgroundDark
                        }
                    }

                    Label {
                        id: merginLink
                        text: __merginApi.apiRoot
                        height: fieldHeight/2
                        color: InputStyle.panelBackgroundDark
                        verticalAlignment: Text.AlignVCenter

                        MouseArea {
                            width: parent.width
                            height: parent.height
                            onClicked: {
                                merginDialog.open()
                            }
                        }
                    }
                }
            }
        }
    }

    MerginSettingDialog {
        id: merginDialog
        onClosing: root.forceActiveFocus()
        height: __androidUtils.isAndroid ? 0 : fieldHeight * 4
        width: __androidUtils.isAndroid ? 0 : window.width - 2 * merginDialog.panelMargin
    }
}
