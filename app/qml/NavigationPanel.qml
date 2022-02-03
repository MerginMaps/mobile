/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.0

import "."  // import InputStyle singleton

Item {
    id: navigationPanel
    property real rowHeight: InputStyle.rowHeight

    property var navigationTargetFeature

    property string featureTitle
    property string featureToGpsDistance: "0"

    property real previewHeight

    readonly property alias isOpen: drawer.opened

    property var _map

    property bool autoFollow: true
    property var calculatedNavigationExtent
    property real previewPanelHeight

    property string mapStateBeforeNavigation

    signal navigationEnded()

    function startNavigation() {
      mapStateBeforeNavigation = _map.state
      _map.state = "navigation"
      drawer.open()
    }

    function endNavigation() {
      if ( _map.state !== "navigation" )
        return;

      autoFollow = true
      updateNavigation()
      autoFollow = false
      _map.state = mapStateBeforeNavigation;
      drawer.close()
      navigationEnded()
    }

    function updateNavigation() {
      if ( _map.state !== "navigation" )
        return;

      _map.navigationHighlightFeature = navigationTargetFeature
      _map.navigationHighlightGpsPosition = __positionKit.positionCoordinate

      var previewPanelHeightRatio = previewPanelHeight / _map.height;
      calculatedNavigationExtent =  __inputUtils.navigationFeatureExtent( _map.navigationHighlightFeature, __positionKit.positionCoordinate, _map.mapSettings, previewPanelHeightRatio );

      if ( autoFollow )
        _map.mapSettings.extent = calculatedNavigationExtent;

      navigationPanel.featureToGpsDistance = __inputUtils.distanceToFeature( __positionKit.positionCoordinate, navigationTargetFeature, _map.mapSettings );
    }

    onAutoFollowChanged: {
      updateNavigation()
    }

    onNavigationTargetFeatureChanged: {
      navigationPanel.featureTitle = __inputUtils.featureTitle( navigationTargetFeature, __loader.project )
      _map.navigationHighlightFeature = navigationTargetFeature
      autoFollow = true;
      updateNavigation()
    }

    Connections {
      target: _map.mapSettings
      onExtentChanged: {
        if ( _map.state === "navigation" && _map.mapSettings.extent !== calculatedNavigationExtent )
          autoFollow = false;
      }
    }

    Connections {
        target: __positionKit
        onPositionChanged: {
          if ( _map.state === "navigation" && navigationTargetFeature )
            updateNavigation();
        }
    }

    Drawer {
      id: drawer

      onClosed: endNavigation()

      Behavior on height {
        PropertyAnimation { properties: "height"; easing.type: Easing.InOutQuad }
      }

      width: parent.width
      height: navigationPanel.previewHeight
      z: 0
      modal: false
      dragMargin: 0 // prevents opening the drawer by dragging.
      edge: Qt.BottomEdge
      closePolicy: Popup.CloseOnEscape // prevents the drawer closing while moving canvas

      Rectangle {
          anchors.fill: parent
          color: InputStyle.clrPanelMain

          Rectangle {
              anchors.fill: parent
              anchors.margins: InputStyle.panelMargin
              anchors.topMargin: 0

              Item {
                  id: header
                  width: parent.width
                  height: navigationPanel.rowHeight
                  Row {
                    id: title
                    height: rowHeight
                    width: parent.width

                    Text {
                        id: titleText
                        height: rowHeight
                        width: parent.width - recenterIconContainer.width - iconContainer.width
                        text: featureTitle
                        font.pixelSize: InputStyle.fontPixelSizeBig
                        color: InputStyle.fontColor
                        font.bold: true
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        elide: Qt.ElideRight
                    }

                    Item {
                        id: recenterIconContainer
                        height: rowHeight
                        width: rowHeight

                        MouseArea {
                            id: recenterIconArea
                            anchors.fill: recenterIconContainer
                            onClicked: autoFollow = true;
                        }

                        Image {
                            id: recenterIcon
                            anchors.fill: parent
                            anchors.margins: rowHeight/4
                            anchors.rightMargin: 0
                            source: InputStyle.zoomToProjectIcon
                            sourceSize.width: width
                            sourceSize.height: height
                            fillMode: Image.PreserveAspectFit
                        }

                        ColorOverlay {
                            anchors.fill: recenterIcon
                            source: recenterIcon
                            color: InputStyle.fontColor
                        }
                    }

                    Item {
                        id: iconContainer
                        height: rowHeight
                        width: rowHeight

                        MouseArea {
                            id: editArea
                            anchors.fill: iconContainer
                            onClicked: endNavigation()
                        }

                        Image {
                            id: icon
                            anchors.fill: parent
                            anchors.margins: rowHeight/4
                            anchors.rightMargin: 0
                            source: InputStyle.closeIcon
                            sourceSize.width: width
                            sourceSize.height: height
                            fillMode: Image.PreserveAspectFit
                        }

                        ColorOverlay {
                            anchors.fill: icon
                            source: icon
                            color: InputStyle.fontColor
                        }
                    }
                  }

                  Rectangle {
                      id: titleBorder
                      width: parent.width
                      height: 1
                      color: InputStyle.fontColor
                      anchors.bottom: title.bottom
                  }

                  Item {
                      id: content
                      width: parent.width
                      anchors.top: header.bottom
                      anchors.bottom: parent.bottom
                      Text {
                        text: "Distance: " + featureToGpsDistance
                        font.pixelSize: InputStyle.fontPixelSizeNormal
                        color: InputStyle.fontColor
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        elide: Qt.ElideRight
                      }
                  }
              }
          }
      }
    }
}
