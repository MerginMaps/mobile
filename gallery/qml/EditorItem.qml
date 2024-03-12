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
import QtQuick.Controls.Basic

import "../app/qml/components"
import "../app/qml/inputs"
import "../app/qml/form/editors"

Item {
  required property string fieldTitle

  property string fieldValue: ""
  property var fieldConfig: ({UseHtml: true})
  property bool fieldShouldShowTitle: checkboxTitle.checked
  property bool fieldIsReadOnly: !checkbox.checked
  property string fieldErrorMessage: checkboxError.checked ? "error" : ""
  property string fieldWarningMessage: checkboxWarning.checked ? "warning" : ""
  property bool fieldRememberValueSupported: checkboxRemember.checked
  property bool fieldRememberValueState: false
  property string fieldHomePath: ""
  property bool fieldValueIsNull: false
  property string fieldAssociatedRelation: ""
  property string fieldFeatureLayerPair: ""
  property variant fieldActiveProject: ({homePath: ""})
  property string field: ""
}
