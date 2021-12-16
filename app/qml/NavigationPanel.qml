/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.0

import "."  // import InputStyle singleton
import "./components" as Components
import lc 1.0

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


    function startNavigation() {
      mapStateBeforeNavigation = _map.state
      _map.state = "navigation"
      drawer.open()
    }

    function endNavigation() {
      _map.state = mapStateBeforeNavigation;
      drawer.close()
    }

    function updateNavigation() {
      var mapSettings = _map.mapSettings;
      var positionKit = _map.positionKit;

      _map.navigationHighlightFeature = __inputUtils.constructNavigationLineFeatureLayerPair( navigationTargetFeature, _map.positionKit.position, __disposableLinesLayer, mapSettings );

      var previewPanelHeightRatio = previewPanelHeight / _map.height;
      calculatedNavigationExtent =  __inputUtils.navigationFeatureExtent( _map.navigationHighlightFeature, mapSettings, previewPanelHeightRatio );

      if ( autoFollow )
        mapSettings.extent = calculatedNavigationExtent;

      var gpsToFeature = __inputUtils.distanceToFeature( _map.positionKit.position, navigationTargetFeature, mapSettings );
      navigationPanel.featureToGpsDistance = gpsToFeature
    }

    onAutoFollowChanged: {
      updateNavigation()
    }

    onNavigationTargetFeatureChanged: {
      navigationPanel.featureTitle = __inputUtils.featureTitle( navigationTargetFeature, __loader.project )
      _map.navigationHighlightFeature = __inputUtils.constructNavigationLineFeatureLayerPair( navigationTargetFeature, _map.positionKit.position, __disposableLinesLayer, _map.mapSettings )
      autoFollow = true;
      updateNavigation()
    }

    Connections {
        target: _map.positionKit
        onPositionChanged: {
          if ( _map.state === "navigation" && navigationTargetFeature )
            updateNavigation();
        }
    }

    Connections {
      target: _map.mapSettings
      onExtentChanged: {
        if ( _map.state === "navigation" && _map.mapSettings.extent !== calculatedNavigationExtent )
          autoFollow = false;
      }
    }

    Drawer {
      id: drawer

      onClosed: {
        _map.navigationHighlightFeature = null;
//        drawerClosed();
      }

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
                        width: parent.width - navigationIconContainer.width - iconContainer.width
                        text: featureTitle
                        font.pixelSize: InputStyle.fontPixelSizeTitle
                        color: InputStyle.fontColor
                        font.bold: true
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        elide: Qt.ElideRight
                    }

                    Item {
                        id: navigationIconContainer
                        height: rowHeight
                        width: rowHeight

                        MouseArea {
                            id: navigationIconArea
                            anchors.fill: navigationIconContainer
                            onClicked: autoFollow = true;
                        }

                        Image {
                            id: navigationIcon
                            anchors.fill: parent
                            anchors.margins: rowHeight/8
                            anchors.rightMargin: 0
                            source: InputStyle.navigateToIcon
                            sourceSize.width: width
                            sourceSize.height: height
                            fillMode: Image.PreserveAspectFit
                        }

                        ColorOverlay {
                            anchors.fill: navigationIcon
                            source: navigationIcon
                            color: InputStyle.fontColor
                        }
                    }

                    Item {
                        id: iconContainer
                        height: rowHeight
                        width: rowHeight
                        visible: !previewPanel.isReadOnly

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
                        anchors.top: recenterBtn.bottom
                        text: "Distance: " + featureToGpsDistance
                        font.pixelSize: InputStyle.fontPixelSizeTitle
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
