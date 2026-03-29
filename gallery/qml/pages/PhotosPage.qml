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

import "../../app/qml/components"
import "../../app/qml/form/editors"
import "../components" as GalleryComponents

Page {

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  ScrollView {
    id: scrollView
    anchors.fill: parent

    contentWidth: availableWidth

    Column {
      width: parent.width
      spacing: 20
      topPadding: 20
      rightPadding: 20
      leftPadding: 20

      GalleryComponents.FormPhotoViewer {
        width: parent.width - parent.leftPadding - parent.rightPadding

        onCapturePhotoClicked: console.log("onCapturePhotoClicked")
        onChooseFromGalleryClicked: console.log("onChooseFromGalleryClicked")
      }

      GalleryComponents.FormPhotoViewer {
        width: parent.width - parent.leftPadding - parent.rightPadding

        photoUrl: "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
        photoState: "valid"
      }

      GalleryComponents.FormPhotoViewer {
        width: 200

        photoUrl: "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
        photoState: "valid"
      }

      GalleryComponents.FormPhotoViewer {
        width: 200

        photoState: "notAvailable"
      }

      ScrollView {
        spacing: 20
        width: parent.width - parent.leftPadding - parent.rightPadding
        height: contentHeight

        ScrollBar.vertical.policy: ScrollBar.AlwaysOff
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn

        ScrollBar.horizontal.visible: false

        Row {
          spacing: 10

          MMPhotoCard {
            size: 120
            imageSource: "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
            text: "This is my feature"
            textVisible: true
          }

          MMPhotoCard {
            size: 120
            imageSource: "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
            text: "This is my feature and added longer text to check how it looks"
            textVisible: true
          }

          MMPhotoCard {
            size: 120
            imageSource: "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
            text: "This is my feature"
            textVisible: false
          }
        }
      }

      MMListSpacer {
        height: __style.margin20
      }

      ScrollView {
        spacing: 20
        width: parent.width - parent.leftPadding - parent.rightPadding
        height: contentHeight

        ScrollBar.vertical.policy: ScrollBar.AlwaysOff
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
        ScrollBar.horizontal.visible: false

        Row {
          spacing: 10

          MMPhotoCard {
            size: 180
            imageSource: "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
            text: "This is my feature"
            textVisible: true
          }

          MMPhotoCard {
            size: 180
            imageSource: "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
            text: "This is my feature and added longer text to check how it looks"
            textVisible: true
          }
        }
      }

      MMListSpacer {
        height: __style.margin20
      }

      ScrollView {
        spacing: 20
        width: parent.width - parent.leftPadding - parent.rightPadding

        ScrollBar.vertical.policy: ScrollBar.AlwaysOff
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
        ScrollBar.horizontal.visible: false

        Row {
          spacing: 10

          MMPhotoCard {
            size: (scrollView.width - scrollView.leftPadding - scrollView.rightPadding) / 2
            imageSource: "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
            text: "This is my feature"
            textVisible: true
          }

          MMPhotoCard {
            size: (scrollView.width - scrollView.leftPadding - scrollView.rightPadding) / 2
            imageSource: "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
            text: "This is my feature and added longer text to check how it looks,and again this my feature and added longer text to check how it looks and again is my feature and added longer text to check how it looks"
            textVisible: true
          }
        }
      }

      MMListSpacer {
        height: __style.margin20
      }
    }
  }
}
