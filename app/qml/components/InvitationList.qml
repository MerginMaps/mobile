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

  signal invitationAccepted(string uuid)
  signal invitationRejected(string uuid)

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
      width: ListView.view.width
      height: InputStyle.rowHeightMedium

      RowLayout {
        id: inviteRow

        anchors {
          left: parent.left
          right: parent.right
          verticalCenter: parent.verticalCenter
        }

        ColumnLayout {
          Layout.fillWidth: true
          Layout.fillHeight: true
          spacing: 0

          // name of the workspace to join
          Text {
            text: model.display

            Layout.fillWidth: true
            Layout.fillHeight: true

            color: InputStyle.fontColor

            font.bold: true
            font.pixelSize: InputStyle.fontPixelSizeBig

            elide: Text.ElideMiddle
          }

          // user role in this workspace
          Text {
            text: model.toolTip
            Layout.fillWidth: true
            Layout.fillHeight: true

            color: InputStyle.secondaryFontColor
            font.pixelSize: InputStyle.fontPixelSizeNormal

            elide: Text.ElideMiddle
          }
        }

        // accept button
        Image {
          Layout.rightMargin: InputStyle.listMargins + InputStyle.iconSizeMedium - InputStyle.smallGap
          Layout.preferredWidth: InputStyle.iconSizeMedium
          Layout.preferredHeight: InputStyle.iconSizeMedium

          source: InputStyle.yesIcon

          MouseArea {
            anchors {
              fill: parent // make the click area bigger
              leftMargin: -InputStyle.buttonClickArea
              topMargin: -InputStyle.buttonClickArea
              rightMargin: -InputStyle.buttonClickArea
              bottomMargin: -InputStyle.buttonClickArea
            }

            onClicked: root.invitationAccepted(model.whatsThis)
          }
        }

        // reject button
        Image {
          Layout.rightMargin: InputStyle.listMargins
          Layout.preferredWidth: InputStyle.iconSizeMedium
          Layout.preferredHeight: InputStyle.iconSizeMedium

          source: InputStyle.noIcon

          MouseArea {
            anchors {
              fill: parent // make the click area bigger
              leftMargin: -InputStyle.buttonClickArea
              topMargin: -InputStyle.buttonClickArea
              rightMargin: -InputStyle.buttonClickArea
              bottomMargin: -InputStyle.buttonClickArea
            }

            onClicked: root.invitationRejected(model.whatsThis)
          }
        }
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
