

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QgsQuick 0.1 as QgsQuick
import "."
// import InputStyle singleton
import lc 1.0

Item {

  property alias handler: codeReaderHandler
  signal invokeQrScanner()

  QtObject {
    id: codeReaderHandler

    // Has to be set for actions with callbacks
    property var itemWidget

    /**
     * TODO
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
     * TODO
     * \param itemWidget editorWidget for modified field to send valueChanged signal.
     */
    property var setValue: function setValue(value) {
      codeReaderHandler.itemWidget.valueChanged(value, value === "" || value === null)
    }
  }
}
