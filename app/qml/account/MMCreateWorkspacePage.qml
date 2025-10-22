/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "../components"
import "../inputs"

MMPage {
  id: root

  property bool showProgress: true

  signal createWorkspaceClicked( string name )

  pageBottomMarginPolicy: MMPage.BottomMarginPolicy.PaintBehindSystemBar

  pageHeader.rightItemContent: MMProgressBar {
    anchors.verticalCenter: parent.verticalCenter

    width: 60 * __dp
    height: 4 * __dp

    color: __style.grassColor
    progressColor: __style.forestColor

    position: 1/3
    visible: root.showProgress
  }

  pageContent: Item {
    width: parent.width
    height: parent.height

    MMScrollView {
      width: parent.width
      height: parent.height

      Column {
        id: dynamicContent

        width: parent.width
        height: childrenRect.height

        Text {
          width: parent.width

          text: qsTr( "Name your workspace" )

          elide: Text.ElideRight

          font: __style.h3
          color: __style.forestColor

          horizontalAlignment: Text.AlignHCenter
        }

        MMListSpacer { height: __style.margin16 }

        Text {
          width: parent.width
          height: paintedHeight

          text: qsTr(
                  "Workspace is a place to store your projects. " +
                  "Colleagues can be invited to your workspace to collaborate on projects. " +
                  "Let’s start by giving it a name." )

          lineHeight: __style.fontLineHeight24
          lineHeightMode: Text.FixedHeight

          font: __style.p5
          color: __style.nightColor
          wrapMode: Text.Wrap
          horizontalAlignment: Text.AlignHCenter
        }

        MMListSpacer { height: __style.margin16 }

        MMTextInput {
          id: workspaceName

          width: parent.width
          title: qsTr( "Workspace name" )
          placeholderText: qsTr( "Your Workspace" )
        }

        MMListSpacer {
          id: scrollBarBottomSpacer
          height: createButton.height + __style.margin16 + __style.safeAreaBottom }
      }
    }

    MMInfoBox {
      width: parent.width

      anchors.bottom: createButton.top
      anchors.bottomMargin: __style.margin40

      // hide the bubble on small screens
      visible: root.height + scrollBarBottomSpacer.height - dynamicContent.height - root.pageHeader.height - 2 * height > 0

      title: qsTr( "A tip from us" )
      description: qsTr( "A good candidate for a workspace name is the name of your team or organisation" )
      descriptionMaxLines: 3

      imageSource: __style.bubbleImage
    }

    MMButton {
      id: createButton

      anchors.bottom: safeAreaSpacer.top
      anchors.bottomMargin: __style.margin8

      width: parent.width

      text: qsTr( "Create workspace" )

      onClicked: root.createWorkspaceClicked( workspaceName.text )
    }

    MMListFooterSpacer {
      id: safeAreaSpacer
      height: __style.safeAreaBottom
      anchors.bottom: parent.bottom
    }
  }

  // show error message under the respective field
  function showErrorMessage( msg ) {
    workspaceName.errorMsg = msg
  }
}
