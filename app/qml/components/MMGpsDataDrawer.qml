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
import QtQuick.Layouts
import "."
import ".."
import lc 1.0

Drawer {
  id: root

  property var title
  property real rowHeight: 67 * __dp

  width: ApplicationWindow.window.width
  height: (mainColumn.height > ApplicationWindow.window.height ? ApplicationWindow.window.height : mainColumn.height) - 20 * __dp
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

    ColumnLayout {

      id: mainColumn

      width: parent.width
      spacing: 40 * __dp

      MMHeader {
        id: header

        rightMarginShift: 0
        backVisible: false

        title: qsTr("GPS info")
        titleFont: __style.h3

        MMRoundButton {
          id: backBtn

          anchors.right: parent.right
          anchors.rightMargin: __style.pageMargins
          anchors.verticalCenter: parent.verticalCenter

          iconSource: __style.closeIcon
          iconColor: __style.forestColor

          bgndColor: __style.lightGreenColor
          bgndHoverColor: __style.mediumGreenColor

          onClicked: root.visible = false
        }
      }

      ScrollView {
        id: scrollView

        Layout.fillWidth: true
        Layout.leftMargin: 20 * __dp
        Layout.rightMargin: 20 * __dp
        Layout.maximumWidth: __style.maxPageWidth
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredHeight: {
          if (ApplicationWindow.window){
            var availableHeight = ApplicationWindow.window.height - header.height
            var totalHeight = scrollColumn.childrenRect.height + (20 * __dp)

            if(totalHeight >= ApplicationWindow.window.height) {
              return availableHeight
            }
            return totalHeight
          }
          return 0
        }
        contentWidth: availableWidth
        contentHeight: scrollColumn.childrenRect.height

        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy: ScrollBar.AlwaysOff

        Column{
          id: scrollColumn

          width: parent.width
          spacing: 0

          Row {
            width: parent.width
            height: rowHeight

            MMGpsDataText{
              titleText: "Source"
              descriptionText: __positionKit.positionProvider ? __positionKit.providerName : qsTr( "No receiver" )
            }

            MMGpsDataText{
              titleText: "Status"
              descriptionText: __positionKit.positionProvider ? __positionKit.providerMessage : ""
              alignmentRight: true
              itemVisible: __positionKit.positionProvider && __positionKit.providerType === "external"
            }
          }

          MMSpacer {}

          Row {
            width: parent.width
            height: rowHeight

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
            height: rowHeight

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
            height: rowHeight

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
            height: rowHeight

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
            height: rowHeight

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
            height: rowHeight

            MMGpsDataText{
              titleText: "GPS antenna height"
              descriptionText: __positionKit.gpsAntennaHeight > 0 ? __positionKit.gpsAntennaHeight.toString(3) + " m" : qsTr( "Not set" )
            }
          }

          Item {
            width: 1
            height: 20 * __dp
          }

          MMButton {
            id: primaryButton

            width: parent.width - 2 * 20 * __dp
            anchors.horizontalCenter: parent.horizontalCenter

            text: qsTr("Manage GPS receivers")

            onClicked: {
              console.log("GPS data drawer button test OK")
            }
          }

          Item {
            width: 2
            height: {
              if (ApplicationWindow.window){
                scrollColumn.childrenRect.height + (20 * __dp) >= ApplicationWindow.window.height ? mainColumn.spacing + (20 * __dp) :  20 * __dp
              }
            }
          }
        }
      }
    }
  }
}

