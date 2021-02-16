/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7

Item {

  property alias handler: codeReaderHandler
  signal invokeQrScanner()

  QtObject {
    id: codeReaderHandler

    // Has to be set for actions with callbacks
    property var itemWidget

    /**
     * Suppose to set `supportDataImport` variable of a feature form. If true, field supports QR code reader.
     * \param name "Name" property of field item. Expecting alias if defined, otherwise field name.
     */
    property var supportImportData: function supportImportData(name) {
      if (!!name && name.toLowerCase().includes("qrcode")) {
        return true
      } else {
        return false
      }
    }

    /**
     * Invokes QR scaner and seves reference to the caller (widget) to save the value afterwards
     * \param itemWidget editorWidget for modified field to send valueChanged signal.
     */
    property var importData: function importData(itemWidget) {
      codeReaderHandler.itemWidget = itemWidget
      if (__androidUtils.isAndroid || __iosUtils.isIos) {
        invokeQrScanner()
      } else {
        // Not implemented/supported yet
      }
    }

    /**
     * Suppose to be called after `importData` function as a callback to set the value to the widget.
     * \param value new current valaue to be set for a widget.
     */
    property var setValue: function setValue(value) {
      codeReaderHandler.itemWidget.valueChanged(value, value === "" || value === null)
    }
  }
}
