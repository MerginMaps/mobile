/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

import QgsQuick 0.1 as QgsQuick
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

    property bool isOpen: false

    property var _map

    property bool autoFollow: true
    property var calculatedNavigationExtent
    property real previewPanelHeight

    property string mapStateBeforeNavigation

    signal drawerClosed()
    signal recenterClicked()

    function startNavigation() {
      mapStateBeforeNavigation = _map.state
      _map.state = "navigation"
    }

    function openDrawer() {
      drawer.open()
      isOpen = true
    }

    function closeDrawer() {
      drawer.close()
      isOpen = false
    }

    function updateNavigation() {
      var mapSettings = _map.mapSettings;
      var positionKit = _map.positionKit;

      _map.navigationHighlightFeature = __inputUtils.constructNavigationLineFeatureLayerPair( navigationTargetFeature, _map.positionKit.position, mapSettings );

      var previewPanelHeightRatio = previewPanelHeight / _map.height;
      calculatedNavigationExtent =  __inputUtils.navigationFeatureExtent( _map.navigationHighlightFeature, mapSettings, previewPanelHeightRatio );

      if ( autoFollow )
        mapSettings.extent = calculatedNavigationExtent;

      var gpsToFeature = __inputUtils.distanceToFeature( _map.positionKit.position, navigationTargetFeature, mapSettings );
      navigationPanel.featureToGpsDistance = gpsToFeature
    }

    onNavigationTargetFeatureChanged: {
      navigationPanel.featureTitle = __inputUtils.featureTitle( navigationTargetFeature, __loader.project )
      _map.navigationHighlightFeature = __inputUtils.constructNavigationLineFeatureLayerPair( navigationTargetFeature, _map.positionKit.position, _map.mapSettings )
      autoFollow = true;
      updateNavigation()
    }

    Connections {
        target: _map.positionKit
        onPositionChanged: {
          if ( _map.isInNavigationState && navigationTargetFeature )
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
        drawerClosed();
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

                  Item {
                      id: title
                      width: parent.width
                      height: parent.height - titleBorder.height
                      Text {
                          id: titleText
                          height: parent.height
                          width: parent.width - rowHeight
                          text: featureTitle
                          font.pixelSize: InputStyle.fontPixelSizeTitle
                          color: InputStyle.fontColor
                          font.bold: true
                          horizontalAlignment: Text.AlignLeft
                          verticalAlignment: Text.AlignVCenter
                          elide: Qt.ElideRight
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
                      Button {
                        id: recenterBtn
                        text: "recenter navigation"
                        onClicked: autoFollow = true
                      }
                      Text {
                        anchors.top: recenterBtn.bottom
                        text: "distance from gps position: " + featureToGpsDistance
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
