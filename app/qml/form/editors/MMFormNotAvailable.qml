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

import "../../components/private" as MMPrivateComponents

/*
 * A disabled widget to replace relation/relation reference widgets
 * when multi-editing
 */

MMPrivateComponents.MMBaseSingleLineInput {
  id: root

  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle
  property string _fieldTitle: parent.fieldTitle

  placeholderText: qsTr( "Not available while editing multiple features" )

  readOnly: true
  shouldShowValidation: false

  title: _fieldShouldShowTitle ? _fieldTitle : ""
}
