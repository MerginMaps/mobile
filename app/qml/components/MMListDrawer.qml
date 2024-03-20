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

/**
 * This is a white drawer with close button that shows items from model with icon + title + optional description
 * You can specify element to show where there are no items (use MMMessage component)
 */
MMDrawer {
  id: root

  property alias listModel: listView.model //! Model must implement count property, e.g. QML ListModel

  property alias noItemsDelegate: noItemsDelegate.sourceComponent /* usually MMMessage */

  property string valueRole: "type"
  property string textRole: "name"
  property string descriptionRole: "" //! optional description role
  property string imageRole: "iconSource"

  property var activeValue /* which value defined by valueRole should be highlighted */

  property bool modelIsEmpty: root.listModel ? root.listModel.count === 0 : true

  signal clicked( var type )

  drawerContent: Item {
    width: parent.width
    height: noItemsDelegate.height + listView.height

    Loader {
      id: noItemsDelegate

      height: root.modelIsEmpty ? ( item?.height ?? 0 ) : 0
      anchors.horizontalCenter: parent.horizontalCenter

      active: root.modelIsEmpty
    }

    ListView {
      id: listView

      width: parent.width
      interactive: root.maxHeightHit ? true : false

      height: Math.min( root.drawerContentAvailableHeight, root.listModel ? root.listModel.count * __style.menuDrawerHeight : 0 )
      maximumFlickVelocity: __androidUtils.isAndroid ? __style.scrollVelocityAndroid : maximumFlickVelocity

      delegate: MMListDrawerItem {

        width: ListView.view.width
        height: __style.menuDrawerHeight

        type: model[root.valueRole]
        text: model[root.textRole]
        description: descriptionRole !== "" ? model[root.descriptionRole] : ""
        iconSource: model[root.imageRole]
        isActive: root.activeValue ? root.activeValue === model[root.valueRole] : false

        onClicked: function( type ) { root.clicked(type); root.visible = false }
      }
    }
  }
}
