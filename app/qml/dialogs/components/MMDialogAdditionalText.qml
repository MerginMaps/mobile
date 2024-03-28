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

//
// MMDialogAdditionalText = use as additionalContent to MMDialogDrawer when needing advanced text
//

Rectangle {

  property alias text: txt.text

  height: childrenRect.height
  // set width (parent.width)

  color: __style.lightGreenColor

  radius: __style.radius16

  MMComponents.MMText {
    id: txt

    width: parent.width

    color: __style.nightColor
    padding: __style.margin20
    font: __style.p5

    wrapMode: Text.Wrap
  }
}
