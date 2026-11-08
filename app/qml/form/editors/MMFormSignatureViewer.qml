/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../../components" as MMComponents
import "../../components/private" as MMPrivateComponents
import "../components/signature" as MMSignatureComponents

/*
 * Signature viewer for feature form. Shows the drawn signature, or an empty-state "Sign now" button.
 * Base class for MMFormSignatureEditor.
 */

MMPrivateComponents.MMBaseInput {
  id: root

  property string signatureUrl: ""

  property alias signatureState: signatureStateGroup.state

  signal trashClicked()
  signal signNowClicked()

  StateGroup {
    id: signatureStateGroup

    states: [
      State { name: "valid" },
      State { name: "notSet" },
      State { name: "notAvailable" }
    ]

    state: "notSet"
  }

  inputContent: Rectangle {
    width: parent.width
    height: __style.row160

    color: __style.polarColor
    radius: __style.radius20

    MMComponents.MMPhoto {
      id: signatureImage

      width: parent.width
      height: parent.height

      visible: signatureStateGroup.state !== "notSet"

      photoUrl: root.signatureUrl
      isLocalFile: root.signatureUrl.startsWith( "file:///")
      cache: false

      fillMode: Image.PreserveAspectFit

      onStatusChanged: {
        if ( status === Image.Error ) {
          __inputUtils.log( "Signature Loading", "Could not load the signature image. It may be missing or invalid: " + root.signatureUrl )
        }
      }

      MMComponents.MMRoundButton {
        anchors {
          right: parent.right
          bottom: parent.bottom
          rightMargin: __style.margin10
          bottomMargin: __style.margin10
        }

        bgndColor: __style.negativeColor
        iconSource: __style.deleteIcon
        iconColor: __style.grapeColor

        visible: root.editState === "enabled" && signatureStateGroup.state !== "notSet"

        onClicked: root.trashClicked()
      }
    }

    MMSignatureComponents.MMSignatureAttachment {
      width: parent.width
      height: parent.height

      visible: signatureStateGroup.state === "notSet"
      enabled: root.editState === "enabled"

      onSignNowClicked: root.signNowClicked()
    }
  }
}
