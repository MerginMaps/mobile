/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Previous implementation was forked from https://github.com/qgis/QGIS on 25th Nov 2022
 * File: qgsquickmapcanvas.qml by (C) 2014 by Matthias Kuhn
 */

import QtQuick
import QtCore
import QtQuick.Controls

import Input 0.1 as Input

//
// MapCanvas QML object is a utility class for rendering map layers and handling user interaction
//

Item {
  id: mapRoot

  // Clicked signal is sent 350ms after click (to avoid mixing it with double click)
  signal clicked( point p )

  signal longPressed( point p )

  // UserInteractedWithMap signal is sent each time user pans/zooms the map
  signal userInteractedWithMap()

  property alias mapSettings: mapRenderer.mapSettings
  property alias isRendering: mapRenderer.isRendering

  // Number of miliseconds to differentiate between normal click (select feature) and double-click (zoom)
  property int doubleClickThresholdMilis: 350

  // Requests map redraw
  function refresh() {
    mapRenderer.clearCache()
    mapRenderer.refresh()
  }

  // Animates movement of map canvas from the current center to newPos.
  function jumpTo( newPos )
  {
    rendererPrivate.freeze('jumpTo')

    let newPosMapCRS = mapRenderer.mapSettings.screenToCoordinate( newPos )
    let oldPosMapCRS = mapRenderer.mapSettings.center

    // Disable animation until initial position is set
    jumpAnimator.enabled = false

    jumpAnimator.px = oldPosMapCRS.x
    jumpAnimator.py = oldPosMapCRS.y

    jumpAnimator.enabled = true

    jumpAnimator.px = newPosMapCRS.x
    jumpAnimator.py = newPosMapCRS.y

    rendererPrivate.unfreeze('jumpTo')
  }

  Item {
    id: jumpAnimator

    property double px: 0
    property double py: 0

    Behavior on py {
      SmoothedAnimation {
        duration: 200
        reversingMode: SmoothedAnimation.Immediate
      }
      enabled: jumpAnimator.enabled
    }

    Behavior on px {
      SmoothedAnimation {
        duration: 200
        reversingMode: SmoothedAnimation.Immediate
      }
      enabled: jumpAnimator.enabled
    }

    onPxChanged: {
      if ( enabled ) {
        mapRenderer.mapSettings.center = mapRenderer.mapSettings.toQgsPoint( Qt.point( px, py ) )
      }
    }

    onPyChanged: {
      if ( enabled ) {
        mapRenderer.mapSettings.center = mapRenderer.mapSettings.toQgsPoint( Qt.point( px, py ) )
      }
    }
  }

  Input.MapCanvasMap {
    id: mapRenderer

    width: mapRoot.width
    height: mapRoot.height

    freeze: false

    QtObject {
      id: rendererPrivate

      property var _freezeMap: ({})

      function freeze( id ) {
        _freezeMap[ id ] = true
        mapRenderer.freeze = true
      }

      function unfreeze( id ) {
        delete _freezeMap[ id ]
        mapRenderer.freeze = Object.keys( _freezeMap ).length !== 0
      }

      function vectorDistance( a, b ) {
        return Math.sqrt( Math.pow( a.x - b.x, 2 ), Math.pow( a.y - b.y, 2 ) )
      }
    }
  }

  PinchArea {
    id: pinchArea

    property string freezeId: 'pinch'

    anchors.fill: parent

    onPinchStarted: {
      mapRoot.userInteractedWithMap()
      rendererPrivate.freeze( freezeId )
    }

    onPinchFinished: {
      rendererPrivate.unfreeze( freezeId )
    }

    onPinchUpdated: function ( pinch ) {
      mapRenderer.pan( pinch.center, pinch.previousCenter )
      mapRenderer.zoom( pinch.center, pinch.previousScale / pinch.scale )
    }

    MouseArea {
      id: mouseArea

      property var initialPosition
      property var previousPosition

      property bool isDragging: false

      property string freezeId: 'drag'

      anchors.fill: parent
      enabled: !pinchArea.pinch.active

      // ignore touchpad scroll, listen only to the mouse wheel
      scrollGestureEnabled: false

      onPressed: function ( mouse ) {
        initialPosition = Qt.point( mouse.x, mouse.y )
        rendererPrivate.freeze( mouseArea.freezeId )
      }

      onReleased: function ( mouse ) {
        let clickPosition = Qt.point( mouse.x, mouse.y )

        if ( !clickDifferentiatorTimer.running ) {
          // this is a simple click

          clickDifferentiatorTimer.clickedPoint = Qt.point( mouse.x, mouse.y )
        }
        else {
          // n-th click in a row (second, third,..)

          // do not emit clicked signal for the second click
          mouse.accepted = true

          clickDifferentiatorTimer.invalidate = true

          mapRenderer.zoom( Qt.point( mouse.x, mouse.y ), 0.4 )
        }

        if ( !isDragging ) {
          clickDifferentiatorTimer.restart()
        }

        previousPosition = null
        initialPosition = null
        isDragging = false

        rendererPrivate.unfreeze( mouseArea.freezeId )
      }

      onPressAndHold: function ( mouse ) {
        mapRoot.longPressed( Qt.point( mouse.x, mouse.y ) )
        clickDifferentiatorTimer.invalidate = true
      }

      onWheel: function ( wheel ) {
        if ( wheel.angleDelta.y > 0 ) {
          mapRenderer.zoom( Qt.point( wheel.x, wheel.y ), 0.67 )
        }
        else {
          mapRenderer.zoom( Qt.point( wheel.x, wheel.y ), 1.5 )
        }
      }

      // drag map canvas
      onPositionChanged: function ( mouse ) {
        let target = Qt.point( mouse.x, mouse.y )

        if ( !previousPosition ) {
          previousPosition = target
          initialPosition = target
          return
        }

        // we need to simulate natural scroll ~> revert movement on x and y
        let reverted_x = previousPosition.x - ( mouse.x - previousPosition.x )
        let reverted_y = previousPosition.y - ( mouse.y - previousPosition.y )

        mapRenderer.pan( previousPosition, Qt.point( reverted_x, reverted_y ) )

        previousPosition = target

        let dragDistance = rendererPrivate.vectorDistance( initialPosition, target )
        if ( dragDistance > drag.threshold ) {
          isDragging = true

          // do not emit click after drag
          clickDifferentiatorTimer.reset()
        }
      }

      onIsDraggingChanged: {
        if ( isDragging ) {
          mapRoot.userInteractedWithMap()
        }
      }

      onCanceled: {
        // pinch took over
        previousPosition = null
        initialPosition = null
        isDragging = false
        rendererPrivate.unfreeze( mouseArea.freezeId )
      }

      Timer {
        id: clickDifferentiatorTimer

        property point clickedPoint
        property bool invalidate

        interval: mapRoot.doubleClickThresholdMilis
        repeat: false

        onTriggered: {
          if ( !invalidate ) {
            mapRoot.clicked( clickedPoint )
          }
          invalidate = false
        }

        function reset() {
          clickDifferentiatorTimer.stop()
          clickDifferentiatorTimer.invalidate = false
        }
      }
    }
  }
}
