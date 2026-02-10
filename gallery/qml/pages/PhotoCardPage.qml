

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

Page {

    Rectangle {
        anchors.fill: parent
        color: __style.lightGreenColor
    }

    ScrollView {
        spacing: 20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.fill: parent
        anchors.rightMargin: 20
        width: parent.width - 30
        height: contentHeight

        ScrollBar.vertical.policy: ScrollBar.AlwaysOff
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn

        Row {
            spacing: 10

            MMPhotoCard {
                width: 120
                height: 120
                imageSource: "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
                text: "This is my feature"
                textVisible: true
            }

            MMPhotoCard {
                width: 120
                height: 120
                imageSource: "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
                text: "This is my feature"
                textVisible: false
            }

            MMPhotoCard {
                width: 120
                height: 120
                imageSource: "/Users/gabrielbolbotina/development/repos/mm/build-macos/app/android/assets/qgis-data/projects/Timisoara-trees/20260204_171556.jpg"
                text: "This is my feature and this is how it looks until now"
                textVisible: true
            }
            MMPhotoCard {
                width: 120
                height: 120
                imageSource: "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
                text: "This is my feature"
                textVisible: true
            }

            MMPhotoCard {
                width: 120
                height: 120
                imageSource: "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
                text: "This is my feature"
                textVisible: false
            }

            MMPhotoCard {
                width: 120
                height: 120
                imageSource: "/Users/gabrielbolbotina/development/repos/mm/build-macos/app/android/assets/qgis-data/projects/Timisoara-trees/20260204_171556.jpg"
                text: "This is my feature and I am proud of it to be shown in this state"
                textVisible: true
            }
        }
    }
}
