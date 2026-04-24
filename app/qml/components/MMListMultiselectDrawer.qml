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

  drawerBottomMargin: listViewComponent.count === 0
    ? ( __style.margin20 + __style.safeAreaBottom )
    : 0

  drawerContent: Item {
    width: parent.width
    height: showFullScreen ? root.drawerContentAvailableHeight : contentLayout.height

    Column {
      id: contentLayout

      width: parent.width
      height: showFullScreen ? parent.height : implicitHeight

      spacing: 0

      I.MMSearchInput {
        id: searchBar

        width: parent.width

        delayedSearch: true

        placeholderText: qsTr( "Search" )

        textFieldBackground.color: __style.lightGreenColor

        visible: root.withSearch

        onSearchTextChanged: root.searchTextChanged( searchBar.searchText )

        textField.onPressed: root.showFullScreen = true
      }

      MMListSpacer { id: searchBarSpacer; height: __style.spacing20; visible: root.withSearch }

      Item {
        width: parent.width
        height: listViewComponent.count === 0 ? emptyStateDelegateLoader.height : listViewComponent.height

        Loader {
          id: emptyStateDelegateLoader

          width: parent.width

          visible: listViewComponent.count === 0
          sourceComponent: defaultEmptyStateComponent
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

            property bool checked: root.isSelected( model[root.valueRole] )

            text: model[root.textRole]
            secondaryText: model[root.secondaryTextRole] ?? ""

            rightContent: MMIcon {
              source: __style.doneCircleIcon
              visible: _delegate.checked
            }

            onClicked: {
              if ( root.multiSelect ) {
                _delegate.checked = !_delegate.checked
                addOrRemoveSelected( model[root.valueRole] )
              }
              else {
                root.selectionFinished( [model[root.valueRole]] )
                root.close()
              }
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

      onClicked: root.selectionFinished( root.selected )
    }
  }

  QtObject {
    id: internal

    property real searchBarVerticalSpace: root.withSearch ? searchBar.height + searchBarSpacer.height : 0
  }

  Component {
    id: defaultEmptyStateComponent

    MMListEmptyLoaderDelegate {}
  }

  // QDate/QDateTime values get parsed to JS Date objects in QML, and they do strict comparison by default, which also
  // checks if the object instance is the same, for us the time value equality is enough
  function isEqualDate( a, b ) {
    if ( a instanceof Date && b instanceof Date )
      return a.getTime() === b.getTime()
    return a === b
  }

  function isSelected( value ) {
    return root.selected.some( x => isEqualDate( x, value ) )
  }

  function addOrRemoveSelected( value ) {
    if ( !isSelected( value ) ) {
      root.selected.push( value )
    }
    else {
      root.selected = root.selected.filter( x => !isEqualDate( x, value )  )
    }
  }
}
