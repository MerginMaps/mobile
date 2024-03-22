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

import "../inputs" as I

// TODO: fix bottom margin (safe area) and when there is the confirm button

MMDrawer {
  id: root

  property alias list: listViewComponent
  property alias emptyStateDelegate: emptyStateDelegateLoader.sourceComponent

  property string textRole: "text"
  property string secondaryTextRole: "secondaryText"
  property string valueRole: "value"

  property bool withSearch: true
  property bool multiSelect: false
  property var selected: [] // in/out property, contains a list of (pre-)selected item values

  property bool showFullScreen: false

  signal searchTextChanged( string searchText )
  signal selectionFinished( var selectedItems )

  drawerBottomMargin: 0

  drawerContent: Item {
    width: parent.width
    height: showFullScreen ? root.drawerContentAvailableHeight : contentLayout.height

    Column {
      id: contentLayout

      width: parent.width
      height: showFullScreen ? parent.height : implicitHeight

      spacing: __style.spacing20

      I.MMSearchInput {
        id: searchBar

        width: parent.width

        placeholderText: qsTr( "Search" )

        bgColor: __style.lightGreenColor
        visible: root.withSearch

        onSearchTextChanged: ( text ) => root.searchTextChanged( text )
      }

      Item {
        width: parent.width
        height: listViewComponent.count === 0 ? emptyStateDelegateLoader.height : listViewComponent.height

        MMScrollView {
          width: parent.width
          height: Math.min( root.drawerContentAvailableHeight - searchBar.height - contentLayout.spacing, contentHeight )

          enabled: contentHeight > height

          Loader {
            id: emptyStateDelegateLoader

            visible: listViewComponent.count === 0

            width: parent.width
          }
        }

        ListView {
          id: listViewComponent

          width: parent.width
          height: Math.min( root.drawerContentAvailableHeight - searchBar.height - contentLayout.spacing, contentHeight )

          interactive: ( contentHeight - bottomMargin ) > height

          clip: true
          maximumFlickVelocity: __androidUtils.isAndroid ? __style.scrollVelocityAndroid : maximumFlickVelocity

          bottomMargin: root.multiSelect ? confirmButton.height + __style.margin8 + __style.margin20 : 0

          delegate: MMListDelegate {
            id: _delegate

            property bool checked: root.selected.includes( model[root.valueRole] )

            text: model[root.textRole]
            secondaryText: model[root.secondaryTextRole] ?? ""

            onClicked: {
              if ( root.multiSelect ) {
                _delegate.checked = !_delegate.checked

                // add or remove the item from the selected features list
                addOrRemoveSelected( model[root.valueRole] )
              }
              else {
                root.selectionFinished( [model[root.valueRole]] )
                root.close()
              }
            }

            rightContent: MMIcon {
              source: __style.doneCircleIcon
              visible: _delegate.checked
            }
          }
        }
      }
    }

    MMButton {
      id: confirmButton

      width: parent.width
      anchors {
        bottom: parent.bottom
        bottomMargin: __style.margin8 + __style.safeAreaBottom
      }

      text: qsTr( "Confirm selection" )
    }
  }

  function addOrRemoveSelected( val ) {
    if ( root.selected.indexOf( val ) === -1 ) {
      root.selected.push( val )
    }
    else {
      root.selected = root.selected.filter( ( x ) => x !== val )
    }
  }
}
