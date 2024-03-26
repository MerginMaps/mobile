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

/*
 * Common text input to use in the app.
 * Disabled state can be achieved by setting `enabled: false`.
 *
 * See MMBaseInput for more properties.
 */

MMBaseInput {
  id: root

  property alias loader: drawerLoader

  property var comboboxModel // use any type of model

  property int currentIndex: -1

  property string textRole: "text"
  property string valueRole: "value"

  onRightActionClicked: { drawerLoader.active = true; drawerLoader.focus = true }
  onContentClicked: { drawerLoader.active = true; drawerLoader.focus = true }

  content: MMComponents.MMText {
    id: textField

    anchors.fill: parent

    font: __style.p5

    color: root.enabled ? __style.nightColor : __style.mediumGreenColor

    text: {
      if ( !comboboxModel ) return "";
      if ( currentIndex < 0 || currentIndex >= comboboxModel.count ) return "";

      return comboboxModel.get( currentIndex )[textRole]
    }
  }

  rightAction: MMComponents.MMIcon {
    property bool pressed: false

    anchors.verticalCenter: parent.verticalCenter

    size: __style.icon24
    source: __style.arrowDownIcon
    color: root.enabled ? __style.forestColor : __style.mediumGreenColor
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
