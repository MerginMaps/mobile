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

import "../../app/qml/form" as Form

Item {

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  Form.MMFormTabBar {
    tabButtonsModel: tabModel
  }

  ListModel {
    id: tabModel

    ListElement {
      Name: "Address of the object"
      Visible: true
      TabIndex: 0
    }

    ListElement {
      Name: "Description of the object xyz"
      Visible: true
      TabIndex: 0
    }

    ListElement {
      Name: "Meta"
      Visible: true
      TabIndex: 0
    }

    ListElement {
      Name: "For future use"
      Visible: true
      TabIndex: 0
    }

    ListElement {
      Name: "For the 2nd stage of survey"
      Visible: true
      TabIndex: 0
    }
  }
}
