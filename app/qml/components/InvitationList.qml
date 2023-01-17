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
import QtQuick.Dialogs
import QtQuick.Layouts
import lc 1.0
import "../"
import "."

Item {
  id: root

  ListView {
    id: listview

    anchors.fill: root
    clip: true
    maximumFlickVelocity: __androidUtils.isAndroid ? InputStyle.scrollVelocityAndroid : maximumFlickVelocity

    model: InvitationsProxyModel {
      id: proxyModel

      invitationsSourceModel: InvitationsModel {
        id: model

        merginApi: __merginApi
      }
    }

    spacing: InputStyle.panelSpacing

    delegate: delegateItem
  }

  Component {
    id: delegateItem

    Item {
      height: InputStyle.rowHeight
      width: ListView.view.width

      ColumnLayout {
        id: delegateContent

        anchors {
          left: parent.left
          leftMargin: InputStyle.listMarginsSmall
          right: parent.right
          rightMargin: InputStyle.listMarginsSmall
          top: parent.top
        }

        height: parent.height * 0.9

        spacing: 0

        Text {
          Layout.fillWidth: true

          color: InputStyle.fontColor
          font.bold: true
          text: model.display

          font.pixelSize: InputStyle.fontPixelSizeNormal

          elide: Text.ElideMiddle
        }
      }

      Text {
        Layout.fillWidth: true

        color: InputStyle.secondaryFontColor
        text: model.toolTip

        font.pixelSize: InputStyle.fontPixelSizeSmall

        elide: Text.ElideMiddle
      }

      Rectangle {
        anchors {
          bottom: parent.bottom
          left: parent.left
          right: parent.right
        }

        height: InputStyle.borderSize
        color: InputStyle.panelBackgroundLight
      }

      MouseArea {
        anchors.fill: parent
        onClicked: root.workspaceChangeRequested( model.whatsThis )
      }
    }
  }


  Component {
    id: loadingSpinnerComponent

    LoadingSpinner {
      x: parent.width / 2 - width / 2
      running: invitationsSourceModel.isLoading
    }
  }
}
