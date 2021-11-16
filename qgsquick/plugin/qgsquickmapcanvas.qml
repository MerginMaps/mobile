/***************************************************************************
 qgsquickmapcanvas.qml
  --------------------------------------
  Date                 : 10.12.2014
  Copyright            : (C) 2014 by Matthias Kuhn
  Email                : matthias@opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.3
import QtQuick.Controls 2.2
import QtQml 2.2
import QgsQuick 0.1 as QgsQuick

Item {
  id: mapArea

  /**
   * When the incrementalRendering property is set to true, the automatic refresh of map canvas during rendering is allowed.
   */
  property alias incrementalRendering: mapCanvasWrapper.incrementalRendering

  /**
   * The isRendering property is set to true while a rendering job is pending for this map canvas map.
   * It can be used to show a notification icon about an ongoing rendering job.
   *
   * This is a readonly property.
   *
   * See also QgsQuickMapCanvasMap::mapSettings
   */
  property alias isRendering: mapCanvasWrapper.isRendering

  /**
   * The mapSettings property contains configuration for rendering of the map.
   *
   * It should be used as a primary source of map settings (and project) for
   * all other components in the application.
   *
   * This is a readonly property.
   *
   * See also QgsQuickMapCanvasMap::mapSettings
   */
  property alias mapSettings: mapCanvasWrapper.mapSettings

  /**
   * What is the minimum distance (in pixels) in order to start dragging map
   */
  property real minimumStartDragDistance: 5 * QgsQuick.Utils.dp

  signal clicked(var mouse)

  /**
   * Freezes the map canvas refreshes.
   *
   * In case of repeated geometry changes (animated resizes, pinch, pan...)
   * triggering refreshes all the time can cause severe performance impacts.
   *
   * If freeze is called, an internal counter is incremented and only when the
   * counter is 0, refreshes will happen.
   * It is therefore important to call freeze() and unfreeze() exactly the same
   * number of times.
   */
  function freeze(id) {
    mapCanvasWrapper.__freezecount[id] = true;
    mapCanvasWrapper.freeze = true;
  }
  function unfreeze(id) {
    delete mapCanvasWrapper.__freezecount[id];
    mapCanvasWrapper.freeze = Object.keys(mapCanvasWrapper.__freezecount).length !== 0;
  }

  QgsQuick.MapCanvasMap {
    id: mapCanvasWrapper
    property var __freezecount: ({})

    anchors.fill: parent
    freeze: false
  }
  PinchArea {
    id: pinchArea
    anchors.fill: parent

    onPinchFinished: {
      unfreeze('pinch');
      mapCanvasWrapper.refresh();
    }
    onPinchStarted: {
      freeze('pinch');
    }
    onPinchUpdated: {
      mapCanvasWrapper.zoom(pinch.center, pinch.previousScale / pinch.scale);
      mapCanvasWrapper.pan(pinch.center, pinch.previousCenter);
    }

    MouseArea {
      id: mouseArea
      property bool __dragging: false
      property point __initialPosition
      property point __lastPosition

      anchors.fill: parent

      onCanceled: {
        unfreezePanTimer.start();
      }
      onClicked: {
        if (mouse.button === Qt.RightButton) {
          var center = Qt.point(mouse.x, mouse.y);
          mapCanvasWrapper.zoom(center, 1.2);
        } else {
          var distance = Math.abs(mouse.x - __initialPosition.x) + Math.abs(mouse.y - __initialPosition.y);
          if (distance < minimumStartDragDistance)
            mapArea.clicked(mouse);
        }
      }
      onDoubleClicked: {
        var center = Qt.point(mouse.x, mouse.y);
        mapCanvasWrapper.zoom(center, 0.8);
      }
      onPositionChanged: {
        // are we far enough to start dragging map? (we want to avoid tiny map moves)
        var distance = Math.abs(mouse.x - __initialPosition.x) + Math.abs(mouse.y - __initialPosition.y);
        if (distance >= minimumStartDragDistance)
          __dragging = true;
        if (__dragging) {
          var currentPosition = Qt.point(mouse.x, mouse.y);
          mapCanvasWrapper.pan(currentPosition, __lastPosition);
          __lastPosition = currentPosition;
        }
      }
      onPressed: {
        __lastPosition = Qt.point(mouse.x, mouse.y);
        __initialPosition = __lastPosition;
        __dragging = false;
        freeze('pan');
      }
      onReleased: {
        unfreeze('pan');
      }
      onWheel: {
        mapCanvasWrapper.zoom(Qt.point(wheel.x, wheel.y), Math.pow(0.8, wheel.angleDelta.y / 60));
      }

      Timer {
        id: unfreezePanTimer
        interval: 500
        repeat: false
        running: false

        onTriggered: unfreeze('pan')
      }
    }
  }
}
