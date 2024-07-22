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

import mm 1.0 as MM

import "./components" as MMAccountComponents
import "../components"
import "../inputs"

MMPage {
  id: root

  property string selectedText: ""

  signal howYouFoundUsSelected( var selectedText )

  ListModel {
    id: sourceListModel

    Component.onCompleted: {
      var items = [
        { name: qsTr( "Search engine (Google, ...)" ), key: "search_engine", icon: __style.searchIcon, submenu: false },
        { name: qsTr( "Blog" ), key: "blog", icon: __style.termsIcon, submenu: false },
        { name: qsTr( "Mouth" ), key: "mouth", icon: __style.mouthIcon, submenu: false },
        { name: qsTr( "QGIS website" ), key: "qgis_website", icon: __style.qgisIcon, submenu: false },
        { name: qsTr( "Application store" ), key: "app_store", icon: __style.subscriptionsIcon, submenu: false },
        { name: qsTr( "Teacher" ), key: "teacher", icon: __style.teacherIcon, submenu: false },
        { name: qsTr( "Conference" ), key: "conference", icon: __style.briefcaseIcon, submenu: false },
        { name: qsTr( "Social media" ), key: "social", icon: __style.socialMediaIcon, submenu: false }
      ];

      var otherItem = { name: qsTr( "Other" ), key: "other", icon: __style.otherIcon, submenu: false };

      shuffleAndAppend( sourceListModel, items );

      sourceListModel.append( otherItem );
    }
  }

  pageHeader {
    title: listView.contentY > -10 * __dp ? internal.pageTitle : ""
    backVisible: false

    rightItemContent: MMProgressBar {
      anchors.verticalCenter: parent.verticalCenter

      width: 60 * __dp
      height: 4 * __dp

      color: __style.grassColor
      progressColor: __style.forestColor

      position: 2/3
    }
  }

  pageBottomMarginPolicy: MMPage.BottomMarginPolicy.PaintBehindSystemBar

  pageContent: Item {

    width: parent.width
    height: parent.height

    MMListView {
      id: listView

      width: parent.width
      height: parent.height

      spacing: __style.spacing12

      topMargin: __style.margin40

      // to reserve some space for the footer button
      bottomMargin: footerButton.height + __style.safeAreaBottom + __style.margin8 + __style.margin20

      currentIndex: -1

      model: sourceListModel

      header: MMText {
        width: ListView.view.width

        text: internal.pageTitle

        font: __style.h3
        color: __style.forestColor

        wrapMode: Text.Wrap

        maximumLineCount: 2
        horizontalAlignment: Text.AlignHCenter

        bottomPadding: __style.margin40
      }

      add: Transition {
               NumberAnimation { properties: "x"; from: 100; duration: 100 }
           }

      addDisplaced: Transition {
               NumberAnimation { properties: "x,y"; duration: 100 }
           }

      delegate: MMAccountComponents.MMIconCheckBoxHorizontal {
        x: model.submenu ? __style.margin20 : 0
        width: model.submenu ? ListView.view.width - __style.margin20 : ListView.view.width

        small: model.submenu

        sourceIcon: model.icon
        text: model.name
        checked: listView.currentIndex === index

        onClicked: {
          let optionUnchecked = listView.currentIndex === index

          if ( model.key === "social" && !internal.socialSubmenuOpened && !optionUnchecked ) {
            // add social options
            let i = model.index
            listView.model.insert( ++i, { name: qsTr( "YouTube" ), key: "youtube", icon: __style.youtubeIcon, submenu: true } )
            listView.model.insert( ++i, { name: qsTr( "Twitter" ), key: "twitter", icon: __style.xTwitterIcon, submenu: true } )
            listView.model.insert( ++i, { name: qsTr( "Facebook" ), key: "facebook", icon: __style.facebookIcon, submenu: true } )
            listView.model.insert( ++i, { name: qsTr( "LinkedIn" ), key: "linkedIn", icon: __style.linkedinIcon, submenu: true } )
            listView.model.insert( ++i, { name: qsTr( "Mastodon" ), key: "mastodon", icon: __style.mastodonIcon, submenu: true } )
            listView.model.insert( ++i, { name: qsTr( "Reddit" ), key: "reddit", icon: __style.redditIcon, submenu: true } )

            listView.positionViewAtIndex( model.index, ListView.Beginning )

            internal.socialSubmenuOpened = true
          }
          else if ( !model.submenu ) {
            removeSocialSubitems()
            internal.socialSubmenuOpened = false
          }

          if ( model.key === "other" ) {
            listView.footer = specifySourceFooterComponent
          }
          else {
            listView.footer = null
          }

          if ( optionUnchecked ) {
            listView.currentIndex = -1
          }
          else {
            root.selectedText = model.key
            listView.currentIndex = index
          }
        }
      }
    }

    MMButton {
      id: footerButton

      width: parent.width

      anchors.bottom: parent.bottom
      anchors.bottomMargin: __style.safeAreaBottom + __style.margin8

      text: qsTr("Continue")

      enabled: {
        if ( listView.currentIndex < 0 ) return false
        if ( listView.model.get(listView.currentIndex).key === "social" ) return false
        if ( ( listView.model.get(listView.currentIndex).key === "other" ) && root.selectedText === "" ) return false
        return true
      }

      onClicked: {
        if ( root.selectedText.length > 0 ) {
          root.howYouFoundUsSelected( root.selectedText )
        }
        else {
          __notificationModel.addError( internal.specifySourceText )
        }
      }
    }
  }

  function removeSocialSubitems() {
    for ( let i = listView.model.count - 1; i >= 0; i-- ) {
      if ( listView.model.get(i).submenu ) {
        listView.model.remove(i, 1)
      }
    }
  }

  QtObject {
    id: internal

    property bool socialSubmenuOpened: false

    readonly property string pageTitle: qsTr("Where did you hear about us?")
    readonly property string specifySourceText: qsTr("Please specify the source")
  }

  Component {
    id: specifySourceFooterComponent

    Column {

      width: ListView.view.width

      MMListSpacer { height: __style.margin20}

      MMTextInput {
        width: parent.width

        title: qsTr( "Source" )
        placeholderText: internal.specifySourceText

        onTextChanged: root.selectedText = text

        Component.onCompleted: textField.forceActiveFocus()
      }
    }
  }

  function shuffleAndAppend( listModel, items )
  {
    items = items
        .map(function( item ) {
            return { value: item, sort: Math.random() };
        })
        .sort(function( a, b ) {
            return a.sort - b.sort;
        })
        .map(function( obj ) {
            return obj.value;
        });

    items.forEach( function( item ) {
        listModel.append( item );
    });
  }
}
