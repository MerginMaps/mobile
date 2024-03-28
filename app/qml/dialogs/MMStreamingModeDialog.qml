/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../components" as MMComponents

MMComponents.MMDrawerDialog {
  id: root

  property bool streamingActive: false

  signal streamingBtnClicked()

  imageSource: root.streamingActive ? __style.streamingBootsOrangeImage : __style.streamingBootsImage
  title: qsTr("Streaming mode")
  description: qsTr("Move around your designated object to record its geometry. You can stop this anytime.")
  primaryButton.text: root.streamingActive ? qsTr("Stop streaming mode") : qsTr("Start streaming mode")
  primaryButton.type: root.streamingActive ? MMComponents.MMButton.Types.Secondary : MMComponents.MMButton.Types.Primary

  onPrimaryButtonClicked: {
    close()
    root.streamingBtnClicked()
  }
}
