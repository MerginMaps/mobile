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
import "../components/private" as MMPrivateComponents

/*
 * Common text input to use in the app.
 * Disabled state can be achieved by setting `enabled: false`.
 *
 * See MMBaseSingleLineInput for more properties.
 */

MMPrivateComponents.MMBaseSingleLineInput {
  id: root

  property alias loader: drawerLoader

  property var comboboxModel // use any type of model

  property int currentIndex: -1

  property string textRole: "text"
  property string valueRole: "value"

  onTextClicked: { drawerLoader.active = true; drawerLoader.focus = true }
  onRightContentClicked: { drawerLoader.active = true; drawerLoader.focus = true }

  textField.readOnly: true

  text: {
    if ( !comboboxModel ) return "";
    if ( currentIndex < 0 || currentIndex >= comboboxModel.count ) return "";

    return comboboxModel.get( currentIndex )[textRole]
  }

  rightContent: MMComponents.MMIcon {
    property bool pressed: false

    size: __style.icon24
    source: drawerLoader.active ? __style.arrowUpIcon : __style.arrowDownIcon
    color: root.iconColor
  }

  Loader {
    id: drawerLoader

    active: false
    asynchronous: true
    sourceComponent: listComponent
  }

  Component {
    id: listComponent

    MMComponents.MMListDrawer {

      ListModel { id: _model }

      drawerHeader.title: root.title

      list.model: _model

      list.delegate: MMComponents.MMListDelegate {
        text: model.text

        rightContent: MMComponents.MMIcon {
          source: __style.doneCircleIcon
          visible: index === root.currentIndex
        }

        onClicked: {
          root.currentIndex = index
          close()
        }
      }

      onClosed: drawerLoader.active = false

      Component.onCompleted: {
        for ( let i = 0; i < root.comboboxModel.count; i++ ) {
          let item = root.comboboxModel.get(i)
          _model.append( { text: item[ textRole ] } )
        }

        open()
      }
    }
  }

  function indexFromValue( value ) {
    for ( let i = 0; i < comboboxModel.count; i++ ) {
      let item = comboboxModel.get(i)
      if ( value === item[root.valueRole] ) {
        return i
      }
    }
    return -1
  }
}
