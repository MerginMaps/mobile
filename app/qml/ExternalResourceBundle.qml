import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Item {

    property alias handler: externalResourceHandler
    property string customPrefix;

    QtObject {
        id: externalResourceHandler

        // field editor widget related to current action
        property var itemWidget

        /**
         * Called when clicked on the gallery icon to choose a file from a gallery.
         * ItemWidget reference is set here and kept for the whole workflow to avoid ambiguity in case of
         * multiple external resource (attachment) fields. All usecases and bundle itself counts with one interaction
         * per one time.
         *
         * The workflow of choosing an image from a gallery starts here and goes as follows:
         * Android gallery even is evoked. When a user chooses image, "imageSelected( selectedImagePath )" is emitted.
         * Then "imageSelected" caught the signal, handles changes and sends signal "valueChanged".
         * \param itemWidget editorWidget for modified field to send valueChanged signal.
         */
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

        /**
         * Called when a photo is taken and confirmed (clicked on check/ok button).
         * Original photo file is renamed with current date time to avoid name conflicts.
         * ItemWidget reference is always set here to avoid ambiguity in case of
         * multiple external resource (attachment) fields.
         * \param itemWidget editorWidget for a modified field to send valueChanged signal.
         * \param prefixToRelativePath depends on widget's config, see more qgsquickexternalwidget.qml
         * \value depends on widget's config, see more in qgsquickexternalwidget.qml
         */
        property var confirmImage: function confirmImage(itemWidget, prefixToRelativePath, value) {
          var newPath = __inputUtils.renameWithDateTime(prefixToRelativePath + "/" + value)
          if (newPath) {
            externalResourceHandler.itemWidget = itemWidget
            var newCurrentValue = QgsQuick.Utils.getRelativePath(newPath, prefixToRelativePath)
            externalResourceHandler.itemWidget.image.currentValue = newCurrentValue
            externalResourceHandler.itemWidget.valueChanged(newCurrentValue, newCurrentValue === "" || newCurrentValue === null)
          }
        }

        property var imageSelected: function imageSelected(imagePath) {
          // if prefixToRelativePath is empty (widget is using absolute path), then use targetDir
          var prefix = (externalResourceHandler.itemWidget.prefixToRelativePath) ?
                externalResourceHandler.itemWidget.prefixToRelativePath:
                externalResourceHandler.itemWidget.targetDir

          var filename = __inputUtils.getFileName(imagePath)
          var absolutePath  = externalResourceHandler.itemWidget.getAbsolutePath(prefix, filename)
          if (!QgsQuick.Utils.fileExists(absolutePath)) {
            __inputUtils.copyFile(imagePath, absolutePath)
          }

          var newValue = externalResourceHandler.itemWidget.prefixToRelativePath ?
                QgsQuick.Utils.getRelativePath(absolutePath, externalResourceHandler.itemWidget.prefixToRelativePath) :
                absolutePath

          externalResourceHandler.itemWidget.valueChanged(newValue, false)
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
            externalResourceHandler.itemWidget.image.currentValue = ""
            externalResourceHandler.itemWidget.valueChanged("", false)
            visible = false
        }
        onNo: {
            externalResourceHandler.itemWidget.image.currentValue = ""
            externalResourceHandler.itemWidget.valueChanged("", false)
            // visible = false called afterwards when onReject
        }
        onRejected: {
           visible = false
        }
    }
}


