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

import "../../app/qml/components" as MMComponents

Item {

  Column {
    anchors {
      fill: parent
      margins: 20
    }

    spacing: 0

    MMComponents.MMListDelegate {
      width: parent.width

      text: "Title of this item"
      secondaryText: "Secondary text for this item"
    }

    MMComponents.MMListDelegate {
      width: parent.width

      text: "Forest near my yard"
      secondaryText: "Forest layer"

      leftContent: MMComponents.MMIcon {
        source: __style.polygonLayerNoColorOverlayIcon
      }
    }

    MMComponents.MMListDelegate {
      width: parent.width

      text: "Title of this item"

      leftContent: MMComponents.MMIcon { source: __style.homeIcon }
      rightContent: MMComponents.MMBadge { text: "Active" }
    }

    MMComponents.MMListDelegate {
      width: parent.width

      text: "Another title of this item"
      secondaryText: "Another secondary text for this item"

      rightContent: MMComponents.MMSwitch {}
    }

    MMComponents.MMListDelegate {
      width: parent.width

      text: "Selected item, a-ha!"

      rightContent: MMComponents.MMIcon { source: __style.doneCircleIcon }
    }

    MMComponents.MMListDelegate {
      width: parent.width

      text: "Title of this item"

      hasLine: false
    }
  }
}

