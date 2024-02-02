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
import "."
import ".."
import lc 1.0

Drawer {
  id: root

  property alias title: title.text
  property int minFeaturesCountToFullScreenMode: 4

  padding: 20 * __dp

  width: ApplicationWindow.window.width
  //height: (mainColumn.height > ApplicationWindow.window.height ? ApplicationWindow.window.height : mainColumn.height) - 20 * __dp
  height: ApplicationWindow.window.height - 200
  edge: Qt.BottomEdge

  Rectangle {
    color: roundedRect.color
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    height: 2 * radius
    anchors.topMargin: -radius
    radius: 20 * __dp
  }

  Rectangle {
    id: roundedRect

    anchors.fill: parent
    color: __style.whiteColor

    Column {
      id: mainColumn

      width: parent.width
      spacing: 40 * __dp
      leftPadding: root.padding
      rightPadding: root.padding
      bottomPadding: root.padding

      Row {

        id: header
        width: parent.width - 2 * root.padding
        anchors.horizontalCenter: parent.horizontalCenter

        Item { width: closeButton.width; height: 1 }

        Text {
          id: title

          anchors.verticalCenter: parent.verticalCenter
          font: __style.t1
          width: parent.width - closeButton.width * 2
          color: __style.forestColor
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideRight
        }

        Image {
          id: closeButton

          source: __style.closeButtonIcon

          MouseArea {
            anchors.fill: parent
            onClicked: root.visible = false
          }
        }
      }

      ScrollView {

        width: parent.width
        height: (7 * (__style.comboBoxItemHeight + 20)) * __dp
        contentWidth: rectangleContent.width
        contentHeight: rectangleContent.height

        Rectangle {
          id: rectangleContent
          width: parent.width - 2 * root.padding
          height: childrenRect.height

          Column{
            width: parent.width
            height: parent.height

            Row {
              width: parent.width
              height: __style.comboBoxItemHeight

              MMGpsDataText{
                titleText: "Source"
                descriptionText: "desc"
              }

              MMGpsDataText{
                titleText: "Status"
                descriptionText: "desc2"
                alignmentRight: true
              }
            }

            MMSpacer {}

            Row {
              width: parent.width
              height: __style.comboBoxItemHeight

              MMGpsDataText{
                titleText: "Latitude"
                descriptionText: "desc"
              }

              MMGpsDataText{
                titleText: "Longitude"
                descriptionText: "desc2"
                alignmentRight: true
              }
            }

            MMSpacer {}

            Row {
              width: parent.width
              height: __style.comboBoxItemHeight

              MMGpsDataText{
                titleText: "X"
                descriptionText: "desc"
              }

              MMGpsDataText{
                titleText: "Y"
                descriptionText: "desc2"
                alignmentRight: true
              }
            }

            MMSpacer {}

            Row {
              width: parent.width
              height: __style.comboBoxItemHeight

              MMGpsDataText{
                titleText: "Horizontal accuracy"
                descriptionText: "desc"
              }

              MMGpsDataText{
                titleText: "Vertical accuracy"
                descriptionText: "desc2"
                alignmentRight: true
              }
            }

            MMSpacer {}

            Row {
              width: parent.width
              height: __style.comboBoxItemHeight

              MMGpsDataText{
                titleText: "Altitude"
                descriptionText: "desc"
              }

              MMGpsDataText{
                titleText: "Satellites (in use/view)"
                descriptionText: "desc2"
                alignmentRight: true
              }
            }

            MMSpacer {}

            Row {
              width: parent.width
              height: __style.comboBoxItemHeight

              MMGpsDataText{
                titleText: "Speed"
                descriptionText: "desc"
              }

              MMGpsDataText{
                titleText: "Last Fix"
                descriptionText: "desc2"
                alignmentRight: true
              }
            }

            MMSpacer {}

            Row {
              width: parent.width
              height: __style.comboBoxItemHeight

              MMGpsDataText{
                titleText: "GPS antenna height"
                descriptionText: "desc"
              }
            }
          }
        }
      }
    }

    MMButton {
      id: primaryButton

      width: parent.width - 2 * 20 * __dp
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: parent.bottom
      anchors.bottomMargin: 20 * __dp

      text: qsTr("Manage GPS receivers")

      onClicked: {
        additionalContent.push( positionProviderComponent )
      }
    }
  }
}
