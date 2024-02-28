/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../components"

MMDrawerDialog {
  id: root

  property bool streamingActive: false

  signal streamingBtnClicked()

  picture: __style.streamingBootsImage
  bigTitle: qsTr("Streaming mode")
  description: qsTr("Move around your designated object to record its geometry. You can stop this anytime.")
  primaryButton: root.streamingActive ? qsTr("Stop streaming mode") : qsTr("Start streaming mode")

  onPrimaryButtonClicked: {
    close()
    root.streamingBtnClicked()
  }
}
