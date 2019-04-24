import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Item {

    property alias handler: externalResourceHandler

    QtObject {
        id: externalResourceHandler

        // field editor widget related to current action
        property var itemWidget

        property var chooseImage: function chooseImage(itemWidget) {
            externalResourceHandler.itemWidget = itemWidget
            if (__androidUtils.isAndroid) {
                __androidUtils.callImagePicker()
            } else {
                fileDialog.open()
            }
        }

        property var previewImage: function previewImage(imagePath) {
            imagePreview.source = "file://" +  imagePath
            imagePreview.width = window.width - 2 * InputStyle.panelMargin
            previewImageWrapper.open()
        }

        property var removeImage: function removeImage(itemWidget, imagePath) {
            externalResourceHandler.itemWidget = itemWidget
            if (QgsQuick.Utils.fileExists(imagePath)) {
              imageDeleteDialog.imagePath = imagePath
              imageDeleteDialog.open()
            } else {
              externalResourceHandler.itemWidget.valueChanged("", false)
            }
        }

        property var imageSelected: function imageSelected(imagePath) {
            var homePath  = featureForm.project ? featureForm.project.homePath : ""
            var fileName = QgsQuick.Utils.getRelativePath(imagePath, homePath)
            if (!fileName) {
                fileName = __inputUtils.getFileName(imagePath)
            }
            if (!QgsQuick.Utils.fileExists(homePath, fileName)) {
                __inputUtils.copyFile(imagePath, homePath + "/" + fileName)
            }
            externalResourceHandler.itemWidget.valueChanged(fileName, false)
            externalResourceHandler.itemWidget = undefined
        }

        property var onFormSave: function onFormSave(itemWidget) {
          __inputUtils.removeFile(itemWidget.sourceToDelete)
          itemWidget.sourceToDelete = ""
        }

        property var onFormCancel: function onFormCanceled(itemWidget) {
          itemWidget.sourceToDelete = ""
        }
    }

    Connections {
        target: __androidUtils
        onImageSelected: externalResourceHandler.imageSelected(imagePath)
    }

    Popup {
        id: previewImageWrapper
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Item {
            anchors.fill: parent
        }

        contentHeight: window.height
        contentWidth: window.width
        contentItem: Image {
            id: imagePreview
            anchors.centerIn: parent
            visible: true
            autoTransform: true
            fillMode: Image.PreserveAspectFit
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr( "Open Image" )
        visible: false
        nameFilters: [ qsTr( "Image files (*.gif *.png *.jpg)" ) ]
        width: window.width
        height: window.height

        onAccepted: externalResourceHandler.imageSelected(fileDialog.fileUrl)
    }

    MessageDialog {
        property string imagePath

        id: imageDeleteDialog
        visible: false
        title: qsTr( "Delete photo" )
        text: qsTr( "Would you like to permanently delete the image file?" )
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Yes | StandardButton.No | StandardButton.Cancel
        onYes: {
            externalResourceHandler.itemWidget.sourceToDelete = imageDeleteDialog.imagePath
            externalResourceHandler.itemWidget.valueChanged("", false)
            visible = false
        }
        onNo: {
            externalResourceHandler.itemWidget.valueChanged("", false)
            // visible = false called afterwards when onReject
        }
        onRejected: {
           visible = false
        }
    }
}


