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
        height: ( 7 * ( __style.comboBoxItemHeight + 20 ) ) * __dp
        contentWidth: rectangleContent.width
        contentHeight: rectangleContent.height

        Rectangle {
          id: rectangleContent
          width: parent.width - (2 * root.padding)
          height: childrenRect.height

          Column{
            width: parent.width
            height: parent.height

            Row {
              width: parent.width
              height: __style.comboBoxItemHeight

              MMGpsDataText{
                titleText: "Source"
                descriptionText: __positionKit.positionProvider ? __positionKit.providerName : qsTr( "No receiver" )
              }

              MMGpsDataText{
                titleText: "Status"
                descriptionText: __positionKit.positionProvider ? __positionKit.providerMessage : ""
                alignmentRight: true
              }
            }

            MMSpacer {}

            Row {
              width: parent.width
              height: __style.comboBoxItemHeight

              MMGpsDataText{
                titleText: "Latitude"
                descriptionText: {
                  if ( !__positionKit.hasPosition || Number.isNaN( __positionKit.latitude ) ) {
                    qsTr( "N/A" )
                  }
                  __positionKit.latitude
                }
              }

              MMGpsDataText{
                titleText: "Longitude"
                descriptionText: {
                  if ( !__positionKit.hasPosition || Number.isNaN( __positionKit.longitude ) ) {
                    qsTr( "N/A" )
                  }
                  __positionKit.longitude
                }
                alignmentRight: true
              }
            }

            MMSpacer {}

            Row {
              width: parent.width
              height: __style.comboBoxItemHeight

              MMGpsDataText{
                titleText: "X"
                descriptionText: {
                  if ( !__positionKit.hasPosition || Number.isNaN( __positionKit.x ) ) {
                    qsTr( "N/A" )
                  }
                  __positionKit.x.toFixed(2)
                }
              }

              MMGpsDataText{
                titleText: "Y"
                descriptionText: {
                  if ( !__positionKit.hasPosition || Number.isNaN( __positionKit.x ) ) {
                    qsTr( "N/A" )
                  }
                  __positionKit.y.toFixed(2)
                }
                alignmentRight: true
              }
            }

            MMSpacer {}

            Row {
              width: parent.width
              height: __style.comboBoxItemHeight

              MMGpsDataText{
                titleText: "Horizontal accuracy"
                descriptionText: {
                  if ( !__positionKit.hasPosition || __positionKit.horizontalAccuracy < 0 ) {
                    return qsTr( "N/A" )
                  }

                  __positionKit.horizontalAccuracy.toFixed(2) + " m"
                }
              }

              MMGpsDataText{
                titleText: "Vertical accuracy"
                descriptionText: {
                  if ( !__positionKit.hasPosition || __positionKit.verticalAccuracy < 0 ) {
                    return qsTr( "N/A" )
                  }

                  __positionKit.verticalAccuracy.toFixed(2) + " m"
                }
                alignmentRight: true
              }
            }

            MMSpacer {}

            Row {
              width: parent.width
              height: __style.comboBoxItemHeight

              MMGpsDataText{
                titleText: "Altitude"
                descriptionText: {
                  if ( !__positionKit.hasPosition || Number.isNaN( __positionKit.altitude ) ) {
                    return qsTr( "N/A" )
                  }
                  __positionKit.altitude.toString() + " m"
                }
              }

              MMGpsDataText{
                titleText: "Satellites (in use/view)"
                descriptionText: {
                  if ( __positionKit.satellitesUsed < 0 || __positionKit.satellitesVisible < 0 )
                  {
                    return qsTr( "N/A" )
                  }

                  __positionKit.satellitesUsed + "/" + __positionKit.satellitesVisible
                }
                alignmentRight: true
              }
            }

            MMSpacer {}

            Row {
              width: parent.width
              height: __style.comboBoxItemHeight

              MMGpsDataText{
                titleText: "Speed"
                descriptionText: {
                  if ( !__positionKit.hasPosition || __positionKit.speed < 0 ) {
                    return qsTr( "N/A" )
                  }

                  __positionKit.speed.toString(2) + " km/h"
                }
              }

              MMGpsDataText{
                titleText: "Last Fix"
                descriptionText: __positionKit.lastRead || qsTr( "N/A" )
                alignmentRight: true
              }
            }

            MMSpacer {}

            Row {
              width: parent.width
              height: __style.comboBoxItemHeight

              MMGpsDataText{
                titleText: "GPS antenna height"
                descriptionText: __positionKit.gpsAntennaHeight > 0 ? __positionKit.gpsAntennaHeight.toString(3) + " m" : qsTr( "Not set" )
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
