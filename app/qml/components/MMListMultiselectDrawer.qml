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

  interactive: !listViewComponent.interactive

  drawerBottomMargin: 0

  drawerContent: Item {
    width: parent.width
    height: {
      if ( root.showFullScreen ) return root.drawerContentAvailableHeight
      if ( listViewComponent.count === 0 ) return emptyStateDelegateLoader.height
      return listViewComponent.height
    }

    Column {
      id: contentLayout

      width: parent.width
      height: showFullScreen ? parent.height : implicitHeight

      spacing: 0

      I.MMSearchInput {
        id: searchBar

        width: parent.width

        placeholderText: qsTr( "Search" )

        textFieldBackground.color: __style.lightGreenColor

        visible: root.withSearch

        onSearchTextChanged: root.searchTextChanged( searchBar.searchText )
      }

      MMListSpacer { id: searchBarSpacer; height: __style.spacing20; visible: root.withSearch }

      Item {
        id: featureList
        width: parent.width
        height: listViewComponent.count === 0 ? emptyStateDelegateLoader.height : listViewComponent.height
          Rectangle
          {
              anchors.fill:parent
              color: "green"
          }
        MMScrollView {
          width: parent.width
          height: Math.min( contentHeight, root.drawerContentAvailableHeight - internal.searchBarVerticalSpace )
          anchors.top: parent.top
          visible: listViewComponent.count === 0

          Loader {
            id: emptyStateDelegateLoader

            visible: listViewComponent.count === 0
            width: parent.width
            anchors.top: parent.top
          }
        }

        MMListView {
          id: listViewComponent

          width: parent.width
          height: Math.min( contentHeight, root.drawerContentAvailableHeight - internal.searchBarVerticalSpace )
          visible: count > 0
          interactive: contentHeight > height

          clip: true
          maximumFlickVelocity: __androidUtils.isAndroid ? __style.scrollVelocityAndroid : maximumFlickVelocity

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

          footer: MMListSpacer { height: __style.safeAreaBottom + __style.margin8 + ( root.multiSelect ? confirmButton.height : 0 ) }
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

      visible: root.multiSelect && listViewComponent.count > 0

      text: qsTr( "Confirm selection" )

      onClicked: {
        root.selectionFinished( root.selected )
      }
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

  QtObject {
    id: internal

    property real searchBarVerticalSpace: root.withSearch ? searchBar.height + searchBarSpacer.height : 0
  }
}
