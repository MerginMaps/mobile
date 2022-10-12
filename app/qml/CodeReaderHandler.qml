/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick
import QtQuick.Controls
import "."  // import InputStyle singleton

Item {

  property alias handler: codeReaderHandler
  signal invokeQrScanner()

  id: codeReaderBundle
  anchors.fill: parent

  QtObject {
    id: codeReaderHandler

    // Has to be set for actions with callbacks
    property var itemWidget

    /**
     * Invokes QR scaner and saves reference to the caller (widget) to save the value afterwards.
     * NOTE: Not supported for WIN yet
     * \param itemWidget editorWidget for modified field to send valueChanged signal.
     */
    property var importData: function importData(itemWidget) {
      codeReaderHandler.itemWidget = itemWidget

      if (!codeReaderLoader.active) {
        if (__inputUtils.acquireCameraPermission())
          codeReaderLoader.active = true
        else {
          return
        }
      }

      codeReaderLoader.item.visible = true
    }

    /**
     * Suppose to be called after `importData` function as a callback to set the value to the widget.
     * \param value new current valaue to be set for a widget.
     */
    property var setValue: function setValue( value ) {
      codeReaderHandler.itemWidget.editorValueChanged( value, value === "" || value === null )
    }
  }


  Loader {
    id: codeReaderLoader
    sourceComponent: cameraComponent
    active: false
  }

  Component {
    id: cameraComponent
    CodeScanner {
      id: codeScanner
      width: codeReaderBundle.width
      height: codeReaderBundle.height

      visible: false
      onScanFinished: function( value ) {
        codeReaderHandler.setValue(value)
      }
    }
  }
}
